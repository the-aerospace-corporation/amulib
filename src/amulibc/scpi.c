/**
 * @file scpi.c
 * @brief SCPI command interface implementation
 *
 * @author CJM28241
 * @date 5/7/2019
 */
#include "scpi.h"

#include <stdio.h>

#include "amu_device.h"
#include "amu_regs.h"
#include "amu_config_internal.h"
#include "amulib_version.h"

#ifdef __AMU_USE_SCPI__

/* ---------- SCPI Module State ---------- */

static uint8_t scpi_channel_list[AMU_MAX_CONNECTED_DEVICES + 1];
static volatile amu_device_t* scpi_amu_dev;

static char scpi_input_buffer[AMULIBC_SCPI_INPUT_BUFFER_LENGTH];
static scpi_error_t scpi_error_queue_data[AMULIBC_SCPI_ERROR_QUEUE_SIZE];

static scpi_t scpi_context;
static scpi_interface_t scpi_interface;

static size_t o_count = 1;

/* ---------- SCPI Layout Constants ---------- */

#define SCPI_Param_amu_notes_t(c, v, b)			SCPI_ParamCopyText(c, v, AMU_NOTES_SIZE, &o_count, b)
#define SCPI_Result_amu_notes_t(c, v)			SCPI_ResultText(c, (char *) v)

#define IVSWEEP_CONFIG_FLOAT_OFFSET		offsetof(ivsweep_config_t, am0)
#define IVSWEEP_CONFIG_SETTINGS_COUNT	IVSWEEP_CONFIG_FLOAT_OFFSET		/*!< leading uint8_t fields, one byte each */
#define IVSWEEP_CONFIG_FLOAT_COUNT		((sizeof(ivsweep_config_t) - IVSWEEP_CONFIG_FLOAT_OFFSET) / sizeof(float))

#define IVSWEEP_META_UINT32_OFFSET		offsetof(ivsweep_meta_t, timestamp)
#define IVSWEEP_META_FLOAT_COUNT		(IVSWEEP_META_UINT32_OFFSET / sizeof(float))
#define IVSWEEP_META_UINT32_COUNT		((sizeof(ivsweep_meta_t) - IVSWEEP_META_UINT32_OFFSET) / sizeof(uint32_t))

/*!< RTD channels in the ADC block; each is measured back as one float */
#define AMU_TSENSOR_COUNT				(sizeof(((adc_channels_t *) 0)->val.tsensors) / sizeof(uint32_t))

/* ---------- SCPI Channel List ---------- */

static scpi_bool_t scpi_append_devices(uint8_t* count, int32_t start, int32_t end) {
	// Appending can happen in either direction (forwards or backwards)
	// Valid list ordering -> (@2:5) or (@5:2)
	int8_t step = (start <= end) ? 1 : -1;

	for (int32_t device = start; ; device += step) {
		if (*count >= AMU_MAX_CONNECTED_DEVICES) {return FALSE;}

		scpi_channel_list[(*count)++] = (uint8_t) device;

		if (device == end) {return TRUE;}
	}
}

// Takes the channel list as an already-read parameter, for handlers that must look at it
// before deciding it is one
static scpi_result_t scpi_parse_channelList(scpi_t *context, scpi_parameter_t *channel_list_param) {
	scpi_bool_t is_range;
	size_t dimensions;
	int32_t start, end;
	uint8_t count = 0;

	// Parse each list entry into start/end
	// A param with format (@0,2:4,7) becomes list [0, 2, 3, 4, 7, AMU_DEVICE_END_LIST]
	for (size_t param_idx = 0;
	     SCPI_EXPR_OK == SCPI_ExprChannelListEntry(context, channel_list_param, param_idx, &is_range, &start, &end, 1, &dimensions);
	     param_idx++) {

		// No range is given, just a single number
		if (!is_range) {end = start;}

		// Reject malformed entries and attempt to add the device to the channel list
		// AMU device lists are 1D as opposed to a switch matrix (2D)
		if ((dimensions != 1) || (start < 0) || (start > AMU_MAX_CONNECTED_DEVICES)
		                      || (end < 0)   || (end > AMU_MAX_CONNECTED_DEVICES)
		                      || !scpi_append_devices(&count, start, end)) {
			scpi_channel_list[count] = AMU_DEVICE_END_LIST;
			return SCPI_RES_ERR;
		}
	}

	scpi_channel_list[count] = AMU_DEVICE_END_LIST;

	// Protected commands cannot target remote devices (device > 0)
	if (SCPI_CmdTag(context) & CMD_USB_ONLY) {
		for (uint8_t i = 0; i < count; i++) {
			if (scpi_channel_list[i] > 0) {
				SCPI_ErrorPush(context, SCPI_ERROR_COMMAND_PROTECTED);
				return SCPI_RES_ERR;
			}
		}
	}

	// Validate that all devices in the channel list exist
	for (uint8_t i = 0; i < count; i++) {
		uint8_t device_num = scpi_channel_list[i];
		if (device_num == 0) {continue;} // Local device always exists

		// Check if remote device exists
		uint8_t twi_address = amu_get_device_address(device_num);
		if (twi_address == AMU_NO_ADDRESS_MATCH) {
			SCPI_ErrorPush(context, SCPI_ERROR_HARDWARE_MISSING);
			return SCPI_RES_ERR;
		}
	}

	return SCPI_RES_OK;
}

static scpi_result_t scpi_get_channelList(scpi_t *context) {
	scpi_parameter_t channel_list_param;

	// No parameter list
	if (!SCPI_Parameter(context, &channel_list_param, FALSE)) {
		scpi_channel_list[0] = 0; // Only include self
		scpi_channel_list[1] = AMU_DEVICE_END_LIST;
		return SCPI_RES_OK;
	}

	return scpi_parse_channelList(context, &channel_list_param);
}

/* ---------- SCPI Shared Routing ---------- */

typedef scpi_bool_t (*scpi_param_fn_t)(scpi_t *context, void *value);
typedef void (*scpi_result_fn_t)(scpi_t *context, void *value);

static scpi_bool_t scpi_routed(scpi_t *context, uint8_t result) {
	if (result != 0) {return TRUE;}

	SCPI_ErrorPush(context, SCPI_ERROR_HARDWARE_MISSING);
	return FALSE;
}

// Remote writes are fire-and-forget but device 0 executes inline, so defer it to last ->
// its work overlaps the remotes' instead of delaying them. Writes emit nothing, so the
// reordering is not observable.
static scpi_result_t scpi_route_write(scpi_t *context, uint16_t cmd, size_t write_len) {
	bool local = false;

	for (uint8_t* device = scpi_channel_list; *device != AMU_DEVICE_END_LIST; device++) {
		if (*device == 0) {local = true; continue;}
		if (!scpi_routed(context, _amu_route_command(*device, cmd, write_len, false))) {return SCPI_RES_ERR;}
	}

	if (local && !scpi_routed(context, _amu_route_command(0, cmd, write_len, false))) {return SCPI_RES_ERR;}

	return SCPI_RES_OK;
}

static scpi_result_t scpi_route_query(scpi_t *context, uint16_t cmd, size_t size, int32_t channel, scpi_result_fn_t result) {
	// Trigger pass: start every remote device before collecting any of them so their execution
	// overlaps. Device 0 is skipped, it runs during the collect pass. Register reads are
	// served straight from the register map -> nothing to trigger.
	if (AMU_CMD_IS_EXEC(cmd)) {
		for (uint8_t* device = scpi_channel_list; *device != AMU_DEVICE_END_LIST; device++) {
			if (*device == 0) {continue;}

			// Payload is the channel byte, so no channel -> send nothing ahead of the command
			if (!scpi_routed(context, _amu_route_trigger(*device, cmd, (channel >= 0) ? 1 : 0))) {return SCPI_RES_ERR;}
		}
	}

	// Collect pass: list order, so the response ordering the host sees is unchanged
	for (uint8_t* device = scpi_channel_list; *device != AMU_DEVICE_END_LIST; device++) {
		uint8_t routed;

		if (*device == 0) {
			// process_cmd both runs the command and stages the result, so device 0 executes
			// here rather than in the trigger pass. It reads its channel out of the transfer
			// reg, which an earlier remote fetch has overwritten -> restage it first
			if (channel >= 0) {scpi_amu_dev->transfer_reg[0] = (uint8_t) channel;}
			routed = _amu_route_command(0, cmd, size, true);
		} else if (AMU_CMD_IS_EXEC(cmd)) {
			routed = _amu_route_collect(*device, cmd, size);		// pre-triggered -> wait, fetch
		} else {
			routed = _amu_route_command(*device, cmd, size, true);	// register read, nothing to trigger
		}

		if (!scpi_routed(context, routed)) {return SCPI_RES_ERR;}

		if (result != NULL) {
			result(context, (void *) scpi_amu_dev->transfer_reg);
		}
	}

	return SCPI_RES_OK;
}

/*
 * Shared body for the scpi_cmd_rw_<type> and scpi_cmd_exec_qry_<type> handlers.
 * param == NULL marks a no-payload (exec/query) command: writes send only the
 * optional channel byte.
 */
static scpi_result_t scpi_cmd_rw(scpi_t *context, size_t size, scpi_param_fn_t param, scpi_result_fn_t result) {
	int32_t channel = -1;
	size_t write_len = (param != NULL) ? size : 0;
	void *payload = (void *) scpi_amu_dev->transfer_reg;

	_amu_transfer_clear(AMU_TRANSFER_REG_SIZE);

	// Channels are placed in first byte of transfer register
	SCPI_CommandNumbers(context, &channel, 1, -1);
	if (channel >= 0) {
		scpi_amu_dev->transfer_reg[0] = (uint8_t) channel;
		payload = (void *) &scpi_amu_dev->transfer_reg[1];
		write_len++;
	}

	// Retrieve any parameters on a write
	if (!context->query && (param != NULL)) {
		if (!param(context, payload)) {
			return SCPI_RES_ERR;
		}
	}

	// Parse channel list with format (@n,...)
	if (scpi_get_channelList(context) == SCPI_RES_ERR) {
		return SCPI_RES_ERR;
	}

	// Set read bit on command
	uint16_t cmd = SCPI_CmdTag(context);
	if (context->query && (AMU_CMD_IS_EXEC(cmd))) {
		cmd |= CMD_READ;
	}

	return context->query ? scpi_route_query(context, cmd, size, channel, result)
	                      : scpi_route_write(context, cmd, write_len);
}

/* ---------- SCPI Generated Per-Type Handlers ---------- */

// scpi_result_<TYPE> is shared by scpi_cmd_rw_<TYPE> and scpi_cmd_exec_qry_<TYPE>, so it's
// generated independently of both - a type can use either macro, or both, without redefining it.
#define SCPI_CMD_RESULT(TYPE)                                                               \
static void scpi_result_##TYPE(scpi_t *context, void *value) {                             \
	SCPI_Result_##TYPE(context, *(TYPE *) value);                                           \
}

#define SCPI_CMD_PARAM(TYPE)                                                                \
static scpi_bool_t scpi_param_##TYPE(scpi_t *context, void *value) {                       \
	return SCPI_Param_##TYPE(context, value, TRUE);                                         \
}

#define SCPI_CMD_RW(TYPE)                                                                   \
scpi_result_t scpi_cmd_rw_##TYPE(scpi_t *context) {                                         \
	return scpi_cmd_rw(context, sizeof(TYPE), scpi_param_##TYPE, scpi_result_##TYPE);    \
}

#define SCPI_CMD_EXEC_QRY(TYPE)                                                             \
scpi_result_t scpi_cmd_exec_qry_##TYPE(scpi_t *context) {                                   \
	return scpi_cmd_rw(context, sizeof(TYPE), NULL, scpi_result_##TYPE);                  \
}

// Write only: a result of NULL stages nothing back. Named off TYPE like the two above, so a
// copy-paste that forgets to change the type is a duplicate definition rather than a silent mispair
#define SCPI_CMD_WRITE(TYPE)                                                                \
scpi_result_t scpi_cmd_write_##TYPE(scpi_t *context) {                                      \
	return scpi_cmd_rw(context, sizeof(TYPE), scpi_param_##TYPE, NULL);                  \
}

#define SCPI_FLOAT_ARRAY_PARAM(TYPE)                                                                             \
static scpi_bool_t scpi_param_##TYPE(scpi_t *context, void *value) {                                            \
	return SCPI_ParamArrayFloat(context, value, sizeof(TYPE) / sizeof(float), &o_count, SCPI_FORMAT_ASCII, TRUE); \
}

#define SCPI_FLOAT_ARRAY_RESULT(TYPE)                                                                            \
static void scpi_result_##TYPE(scpi_t *context, void *value) {                                                  \
	SCPI_ResultArrayFloat(context, (float *) value, sizeof(TYPE) / sizeof(float), SCPI_FORMAT_ASCII);             \
}

SCPI_CMD_PARAM(uint8_t)                  SCPI_CMD_RESULT(uint8_t)                  SCPI_CMD_RW(uint8_t)          SCPI_CMD_EXEC_QRY(uint8_t)
SCPI_CMD_PARAM(uint16_t)                 SCPI_CMD_RESULT(uint16_t)                 SCPI_CMD_RW(uint16_t)         SCPI_CMD_EXEC_QRY(uint16_t)
SCPI_CMD_PARAM(uint32_t)                 SCPI_CMD_RESULT(uint32_t)                 SCPI_CMD_RW(uint32_t)         SCPI_CMD_EXEC_QRY(uint32_t)
SCPI_CMD_PARAM(int32_t)                  SCPI_CMD_RESULT(int32_t)                  SCPI_CMD_RW(int32_t)
SCPI_CMD_PARAM(float)                    SCPI_CMD_RESULT(float)                    SCPI_CMD_RW(float)            SCPI_CMD_EXEC_QRY(float)
SCPI_CMD_PARAM(amu_notes_t)              SCPI_CMD_RESULT(amu_notes_t)              SCPI_CMD_RW(amu_notes_t)

SCPI_FLOAT_ARRAY_PARAM(amu_pid_t)        SCPI_FLOAT_ARRAY_RESULT(amu_pid_t)        SCPI_CMD_RW(amu_pid_t)
SCPI_FLOAT_ARRAY_PARAM(amu_coeff_t)      SCPI_FLOAT_ARRAY_RESULT(amu_coeff_t)      SCPI_CMD_RW(amu_coeff_t)
SCPI_FLOAT_ARRAY_PARAM(quad_photo_sensor_t) SCPI_FLOAT_ARRAY_RESULT(quad_photo_sensor_t) SCPI_CMD_RW(quad_photo_sensor_t)
SCPI_FLOAT_ARRAY_PARAM(press_data_t)     SCPI_FLOAT_ARRAY_RESULT(press_data_t)     SCPI_CMD_RW(press_data_t)
SCPI_FLOAT_ARRAY_PARAM(amu_int_volt_t)   SCPI_FLOAT_ARRAY_RESULT(amu_int_volt_t)   SCPI_CMD_RW(amu_int_volt_t)
                                         SCPI_FLOAT_ARRAY_RESULT(amu_meas_t)                                     SCPI_CMD_EXEC_QRY(amu_meas_t)

/* ---------- SCPI Command Handlers ---------- */

scpi_result_t scpi_cmd_execute(scpi_t *context) {
	int32_t* commandNumber = (int32_t *) scpi_amu_dev->transfer_reg;

	SCPI_CommandNumbers(context, commandNumber, 1, -1);

	if (scpi_get_channelList(context) == SCPI_RES_ERR) {
		return SCPI_RES_ERR;
	}

	// A command number, when the header carried one, is the single byte of payload
	size_t len = (*commandNumber == -1) ? 0 : sizeof(uint8_t);

	return scpi_route_write(context, SCPI_CmdTag(context), len);
}

scpi_result_t scpi_cmd_read_ptr(scpi_t *context) {
	uint8_t numPoints;

	if (scpi_get_channelList(context) == SCPI_RES_ERR) {
		return SCPI_RES_ERR;
	}

	for (uint8_t* device = scpi_channel_list; *device != AMU_DEVICE_END_LIST; device++) {
		_amu_route_command(*device, AMU_REG_SWEEP_CONFIG_NUM_POINTS, sizeof(numPoints), true);

		// Clamp num points
		numPoints = scpi_amu_dev->transfer_reg[0];
		if (numPoints > IVSWEEP_MAX_POINTS) {numPoints = IVSWEEP_MAX_POINTS;}

		switch ((AMU_REG_DATA_PTR_t) SCPI_CmdTag(context)) {
			case AMU_REG_DATA_PTR_TIMESTAMP:
				_amu_route_command(*device, SCPI_CmdTag(context), numPoints * sizeof(uint32_t), true);
				SCPI_ResultArrayUInt32(context, (uint32_t *) &scpi_amu_dev->transfer_reg[0], numPoints, SCPI_FORMAT_ASCII);
				break;
			case AMU_REG_DATA_PTR_VOLTAGE:
			case AMU_REG_DATA_PTR_CURRENT:
			case AMU_REG_DATA_PTR_SS_YAW:
			case AMU_REG_DATA_PTR_SS_PITCH:
				_amu_route_command(*device, SCPI_CmdTag(context), numPoints * sizeof(float), true);
				SCPI_ResultArrayFloat(context, (float *) &scpi_amu_dev->transfer_reg[0], numPoints, SCPI_FORMAT_ASCII);
				break;
			case AMU_REG_DATA_PTR_SWEEP_CONFIG:
				_amu_route_command(*device, SCPI_CmdTag(context), sizeof(ivsweep_config_t), true);
				SCPI_ResultArrayUInt8(context, (uint8_t *) &scpi_amu_dev->transfer_reg[0], IVSWEEP_CONFIG_SETTINGS_COUNT, SCPI_FORMAT_ASCII);
				SCPI_ResultArrayFloat(context, (float *) &scpi_amu_dev->transfer_reg[IVSWEEP_CONFIG_FLOAT_OFFSET], IVSWEEP_CONFIG_FLOAT_COUNT, SCPI_FORMAT_ASCII);
				break;
			case AMU_REG_DATA_PTR_SWEEP_META:
				_amu_route_command(*device, SCPI_CmdTag(context), sizeof(ivsweep_meta_t), true);
				SCPI_ResultArrayFloat(context, (float *) &scpi_amu_dev->transfer_reg[0], IVSWEEP_META_FLOAT_COUNT, SCPI_FORMAT_ASCII);
				SCPI_ResultArrayUInt32(context, (uint32_t *) &scpi_amu_dev->transfer_reg[IVSWEEP_META_UINT32_OFFSET], IVSWEEP_META_UINT32_COUNT, SCPI_FORMAT_ASCII);
				break;
			case AMU_REG_DATA_PTR_SUNSENSOR:
				_amu_route_command(*device, SCPI_CmdTag(context), sizeof(ss_angle_t), true);
				SCPI_ResultArrayFloat(context, (float *) &scpi_amu_dev->transfer_reg[0], sizeof(ss_angle_t) / sizeof(float), SCPI_FORMAT_ASCII);
				break;
			case AMU_REG_DATA_PTR_PRESSURE:
				_amu_route_command(*device, SCPI_CmdTag(context), sizeof(press_data_t), true);
				SCPI_ResultArrayFloat(context, (float *) &scpi_amu_dev->transfer_reg[0], sizeof(press_data_t) / sizeof(float), SCPI_FORMAT_ASCII);
				break;
			default: break;
		}

	}

	return SCPI_RES_OK;
}

scpi_result_t scpi_cmd_write_sweep_ptr(scpi_t *context) {
	if (!SCPI_ParamArrayFloat(context, (void *) scpi_amu_dev->transfer_reg, AMU_TRANSFER_REG_SIZE / sizeof(float), &o_count, SCPI_FORMAT_ASCII, TRUE)) {
		return SCPI_RES_ERR;
	}

	if (scpi_get_channelList(context) == SCPI_RES_ERR) {
		return SCPI_RES_ERR;
	}

	for (uint8_t* device = scpi_channel_list; *device != AMU_DEVICE_END_LIST; device++) {
		if (o_count > IVSWEEP_MAX_POINTS) {return SCPI_RES_ERR;}
		_amu_route_command(*device, SCPI_CmdTag(context), (o_count * sizeof(float)), false);
	}

	return SCPI_RES_OK;
}

static scpi_bool_t scpi_param_ivsweep_config_t(scpi_t *context, void *value) {
	uint32_t sweepSettings[IVSWEEP_CONFIG_SETTINGS_COUNT];
	uint8_t *bytes = (uint8_t *) value;

	if (!SCPI_ParamArrayUInt32(context, sweepSettings, IVSWEEP_CONFIG_SETTINGS_COUNT, &o_count, SCPI_FORMAT_ASCII, TRUE)) {
		return FALSE;
	}
	if (!SCPI_ParamFloat(context, bytes + offsetof(ivsweep_config_t, am0), TRUE)) {
		return FALSE;
	}
	if (!SCPI_ParamFloat(context, bytes + offsetof(ivsweep_config_t, area), TRUE)) {
		return FALSE;
	}

	// libscpi's smallest array parse is uint32; narrow to the struct's uint8_t fields
	for (size_t i = 0; i < IVSWEEP_CONFIG_SETTINGS_COUNT; i++) {
		bytes[i] = (uint8_t) sweepSettings[i];
	}
	return TRUE;
}

SCPI_CMD_WRITE(ivsweep_config_t)

static scpi_bool_t scpi_param_ivsweep_meta_t(scpi_t *context, void *value) {
	uint8_t *bytes = (uint8_t *) value;

	if (!SCPI_ParamArrayFloat(context, (void *) bytes, IVSWEEP_META_FLOAT_COUNT, &o_count, SCPI_FORMAT_ASCII, TRUE)) {
		return FALSE;
	}

	// Only the timestamp is host settable; crc is computed on the device and stays zeroed
	return SCPI_ParamUInt32(context, bytes + IVSWEEP_META_UINT32_OFFSET, TRUE);
}

SCPI_CMD_WRITE(ivsweep_meta_t)

scpi_result_t scpi_cmd_led(scpi_t *context) {
	if (!SCPI_ParamUInt32(context, (void *) scpi_amu_dev->transfer_reg, TRUE)) {
		return SCPI_RES_ERR;
	}

	if (scpi_get_channelList(context) == SCPI_RES_ERR) {
		return SCPI_RES_ERR;
	}

	for (uint8_t* device = scpi_channel_list; *device != AMU_DEVICE_END_LIST; device++) {
		_amu_route_command(*device, (CMD_t) (SCPI_CmdTag(context) + scpi_amu_dev->transfer_reg[0]), sizeof(uint8_t), false);
	}

	return SCPI_RES_OK;
}

// cmd_raw is the program header only, length bounded and not null terminated, so it cannot be searched with strstr
static bool scpi_header_has_raw(scpi_t *context) {
	const char* header = context->param_list.cmd_raw.data;
	size_t length = context->param_list.cmd_raw.length;

	static const char raw[] = "RAW";
	size_t raw_len = sizeof(raw) - 1;

	for (size_t i = 0; (i + raw_len) <= length; i++) {
		if (memcmp(&header[i], raw, raw_len) == 0) {
			return true;
		}
	}

	return false;
}

scpi_result_t scpi_cmd_measure_channel(scpi_t *context) {
	int32_t channel = -1;

	_amu_transfer_clear(AMU_TRANSFER_REG_SIZE);

	SCPI_CommandNumbers(context, &channel, 1, -1);

	if (channel == -1) {
		// No channel in the header, so the command tag carries it in its branch nibble
		channel = AMU_GET_CMD_BRANCH(SCPI_CmdTag(context));
	} else {
		if ((channel < 0) || (channel >= AMU_ADC_CH_NUM)) {
			return SCPI_RES_ERR;
		}

		scpi_amu_dev->transfer_reg[0] = (uint8_t) channel;
	}
	if (!context->query) {
		return SCPI_RES_ERR;
	}

	if (scpi_get_channelList(context) == SCPI_RES_ERR) {
		return SCPI_RES_ERR;
	}

	// Set read bit on command
	uint16_t cmd = SCPI_CmdTag(context);
	if (AMU_CMD_IS_EXEC(cmd)) {
		cmd |= CMD_READ;
	}

	bool raw = scpi_header_has_raw(context);

	for (uint8_t* device = scpi_channel_list; *device != AMU_DEVICE_END_LIST; device++) {
		// The measurement is returned converted, in the transfer register
		if (!scpi_routed(context, _amu_route_command(*device, cmd, sizeof(float), true))) {return SCPI_RES_ERR;}

		if (raw) {
			_amu_route_command(*device, (CMD_t) (AMU_REG_ADC_DATA + (channel * sizeof(uint32_t))), sizeof(uint32_t), true);
			SCPI_ResultUInt32Base(context, transfer_read_uint32_t(), 16);
	 	} else {
			SCPI_ResultFloat(context, transfer_read_float());
		}
	}

	return SCPI_RES_OK;
}

scpi_result_t scpi_cmd_measure_ss_channel(scpi_t *context) {
	return scpi_cmd_measure_channel(context);
}

scpi_result_t scpi_cmd_measure_active_ch(scpi_t *context) {
	if (scpi_get_channelList(context) == SCPI_RES_ERR) {
		return SCPI_RES_ERR;
	}

	uint16_t* activeChannels = (uint16_t *) scpi_amu_dev->transfer_reg;

	for (uint8_t* device = scpi_channel_list; *device != AMU_DEVICE_END_LIST; device++) {
		uint8_t numChannels = 0;

		_amu_route_command(*device, AMU_REG_SYSTEM_ADC_ACTIVE_CHANNELS, sizeof(uint16_t), true);

		for (uint16_t i = 0; i < AMU_ADC_CH_NUM; i++) {
			if (*activeChannels & (1 << i)) {numChannels++;}
		}

		_amu_route_command(*device, (SCPI_CmdTag(context) | CMD_READ), numChannels * sizeof(float), true);

		if (numChannels > 0) {
			SCPI_ResultArrayFloat(context, (float *) scpi_amu_dev->transfer_reg, numChannels, SCPI_FORMAT_ASCII);
		}
	}

	return SCPI_RES_OK;
}

static void scpi_result_tsensors(scpi_t *context, void *value) {
	SCPI_ResultArrayFloat(context, (float *) value, AMU_TSENSOR_COUNT, SCPI_FORMAT_ASCII);
}

scpi_result_t scpi_cmd_measure_tsensors(scpi_t *context) {
	if (scpi_get_channelList(context) == SCPI_RES_ERR) {
		return SCPI_RES_ERR;
	}

	return scpi_route_query(context, (SCPI_CmdTag(context) | CMD_READ), AMU_TSENSOR_COUNT * sizeof(float), -1, scpi_result_tsensors);
}

// Transfer length for each string command, shared by the read and write paths
static size_t scpi_str_length(uint16_t tag) {
	switch (tag) {
		case CMD_SYSTEM_FIRMWARE:		return AMU_FIRMWARE_STR_LEN;
		case CMD_SYSTEM_SERIAL_NUM:		return AMU_SERIALNUM_STR_LEN;
		case CMD_SYSTEM_AMULIB:			return AMU_AMULIB_STR_LEN;
		case CMD_DUT_MANUFACTURER:		return sizeof(scpi_amu_dev->amu_regs->dut.manufacturer);
		case CMD_DUT_MODEL:				return sizeof(scpi_amu_dev->amu_regs->dut.model);
		case CMD_DUT_TECHNOLOGY:		return sizeof(scpi_amu_dev->amu_regs->dut.technology);
		case CMD_DUT_SERIAL_NUMBER:		return sizeof(scpi_amu_dev->amu_regs->dut.serial);
		case CMD_DUT_NOTES:				return AMU_NOTES_SIZE;
		default:						return 0;
	}
}

scpi_result_t scpi_cmd_query_str(scpi_t *context) {
	uint16_t tag = SCPI_CmdTag(context);

	// CMD_READ is bit 7, so it would corrupt an 8 bit register address rather than flag a read
	if (AMU_CMD_IS_REG(tag)) {
		SCPI_ErrorPush(context, SCPI_ERROR_UNDEFINED_HEADER);
		return SCPI_RES_ERR;
	}

	size_t len = scpi_str_length(tag);
	if (len == 0) {
		SCPI_ErrorPush(context, SCPI_ERROR_UNDEFINED_HEADER);
		return SCPI_RES_ERR;
	}

	// Parameters come off the stream in order: the text to write, then any (@n,...) channel list
	if (!context->query) {
		if (!SCPI_ParamCopyText(context, (void *) scpi_amu_dev->transfer_reg, AMU_TRANSFER_REG_SIZE, &o_count, TRUE)) {
			return SCPI_RES_ERR;
		}
	}

	if (scpi_get_channelList(context) == SCPI_RES_ERR) {
		return SCPI_RES_ERR;
	}

	if (context->query) {tag |= CMD_READ;}

	for (uint8_t* device = scpi_channel_list; *device != AMU_DEVICE_END_LIST; device++) {
		if (context->query) {_amu_transfer_clear(len);}

		if (!scpi_routed(context, _amu_route_command(*device, tag, len, context->query))) {return SCPI_RES_ERR;}

		if (context->query) {
			// Bound the string at the requested length; the device need not have null terminated
			scpi_amu_dev->transfer_reg[len - 1] = '\0';
			SCPI_ResultText(context, (const char*) scpi_amu_dev->transfer_reg);
		}
	}

	return SCPI_RES_OK;
}

scpi_result_t scpi_cmd_twi_scan(scpi_t *context) {
	if (!context->query) {
		return SCPI_RES_ERR;
	}

	scpi_parameter_t param;
	scpi_result_t channel_list_rv;
	uint32_t start_address = 0;
	uint32_t end_address = 0;

	// An optional inclusive address range precedes the channel list. A leading parameter that is
	// not a number is the channel list itself, and is handed on rather than consumed
	scpi_bool_t have_param = SCPI_Parameter(context, &param, FALSE);

	if (have_param && SCPI_ParamIsNumber(&param, FALSE)) {
		if (!SCPI_ParamToUInt32(context, &param, &start_address)) {
			SCPI_ErrorPush(context, SCPI_ERROR_DATA_TYPE_ERROR);
			return SCPI_RES_ERR;
		}
		if (!SCPI_ParamUInt32(context, &end_address, TRUE)) {
			return SCPI_RES_ERR;
		}
		channel_list_rv = scpi_get_channelList(context);
	} else if (have_param) {
		channel_list_rv = scpi_parse_channelList(context, &param);
	} else {
		channel_list_rv = scpi_get_channelList(context);
	}

	if (channel_list_rv == SCPI_RES_ERR) {
		return SCPI_RES_ERR;
	}

	// Range endpoints are the payload; zero for either leaves the choice to the device
	scpi_amu_dev->transfer_reg[0] = (uint8_t) start_address;
	scpi_amu_dev->transfer_reg[1] = (uint8_t) end_address;

	for (uint8_t* device = scpi_channel_list; *device != AMU_DEVICE_END_LIST; device++) {
		if (*device == AMU_THIS_DEVICE) {
			_amu_route_command(*device, (SCPI_CmdTag(context) | CMD_READ), 2 * sizeof(uint8_t), context->query);
			SCPI_ResultInt8(context, amu_get_num_devices());
			for (uint8_t i = 0; i < amu_get_num_devices(); i++) {
				if (i == AMU_THIS_DEVICE) {
					SCPI_ResultUInt8(context, scpi_amu_dev->twi_address);
				} else {
					SCPI_ResultUInt8(context, amu_get_device_address(i));
				}
			}
		}
	}

	return SCPI_RES_OK;
}

/* ---------- SCPI Extended Commands ---------- */

/*
 * Shared body for every extended command. The 16-bit extended value occupies bytes
 * 0-1 of the transfer register and any payload has already been staged from byte 2
 * by the caller. The outer command is always CMD_SYSTEM_EXTENDED, so read/write
 * still comes from its bit 7.
 *
 * inLen and outLen are separate because a firmware chunk is large going in and four
 * bytes coming back; _amu_route_command assumes one length for both.
 *
 * Firmware transfer is single device: sequencing several through one transfer register
 * would leave them in mixed states on a partial failure, and a host looping device by
 * device does the same work with far simpler recovery. Queries are single device too,
 * for a plainer reason - the reply lands in the transfer register and the caller reads
 * it once, so a second device would overwrite the first one's answer. Everything else
 * fans out like an ordinary write.
 */
static scpi_result_t scpi_cmd_extended(scpi_t *context, CMD_EXT_t ext, size_t inLen, size_t outLen) {
	if (scpi_get_channelList(context) == SCPI_RES_ERR) {
		return SCPI_RES_ERR;
	}

	bool one_device = context->query || (AMU_GET_EXT_ROOT(ext) == CMD_EXT_FIRMWARE);

	if (one_device && (scpi_channel_list[0] != AMU_DEVICE_END_LIST) && (scpi_channel_list[1] != AMU_DEVICE_END_LIST)) {
		SCPI_ErrorPush(context, SCPI_ERROR_INVALID_SUFFIX);
		return SCPI_RES_ERR;
	}

	uint16_t cmd = CMD_SYSTEM_EXTENDED;
	if (context->query) {cmd |= CMD_READ;}

	// Staged last: the payload is already in place from byte 2
	uint16_t ext_value = (uint16_t) ext;
	memcpy((void *) scpi_amu_dev->transfer_reg, &ext_value, sizeof(ext_value));

	// A routed write sends transferLen bytes and ignores inLen, so a write has to carry its
	// request size there or the extended command id never reaches the remote and the device
	// dispatches on whatever its transfer register still held
	size_t transfer_len = context->query ? outLen : inLen;

	if (one_device) {
		return scpi_routed(context, _amu_route_command_data(scpi_channel_list[0], cmd, inLen, transfer_len, context->query))
			? SCPI_RES_OK : SCPI_RES_ERR;
	}

	// The staged bytes are the same for every target, so a write just repeats the route
	for (uint8_t *device = scpi_channel_list; *device != AMU_DEVICE_END_LIST; device++) {
		if (!scpi_routed(context, _amu_route_command_data(*device, cmd, inLen, transfer_len, false))) {
			return SCPI_RES_ERR;
		}
	}

	return SCPI_RES_OK;
}

scpi_result_t scpi_cmd_ble_state(scpi_t *context) {
	uint8_t state;

	if (!context->query) {
		uint32_t param;
		if (!SCPI_ParamUInt32(context, &param, TRUE)) {return SCPI_RES_ERR;}
		scpi_amu_dev->transfer_reg[AMU_EXT_PAYLOAD] = (param != 0) ? 1 : 0;
	}

	size_t inLen = context->query ? AMU_EXT_PAYLOAD : AMU_EXT_PAYLOAD + sizeof(uint8_t);

	if (scpi_cmd_extended(context, CMD_EXT_BLE_STATE, inLen, context->query ? sizeof(uint8_t) : 0) != SCPI_RES_OK) {
		return SCPI_RES_ERR;
	}

	if (context->query) {
		state = scpi_amu_dev->transfer_reg[0];
		SCPI_ResultUInt8(context, state);
	}

	return SCPI_RES_OK;
}

scpi_result_t scpi_cmd_ble_passkey(scpi_t *context) {
	if (!context->query) {
		uint32_t passkey;
		if (!SCPI_ParamUInt32(context, &passkey, TRUE)) {return SCPI_RES_ERR;}
		memcpy((void *) &scpi_amu_dev->transfer_reg[AMU_EXT_PAYLOAD], &passkey, sizeof(passkey));
	}

	size_t inLen = context->query ? AMU_EXT_PAYLOAD : AMU_EXT_PAYLOAD + sizeof(uint32_t);

	if (scpi_cmd_extended(context, CMD_EXT_BLE_PASSKEY, inLen, context->query ? sizeof(uint32_t) : 0) != SCPI_RES_OK) {
		return SCPI_RES_ERR;
	}

	if (context->query) {
		SCPI_ResultUInt32Base(context, transfer_read_uint32_t(), 10);
	}

	return SCPI_RES_OK;
}

scpi_result_t scpi_cmd_firmware_begin(scpi_t *context) {
	uint32_t image_size;

	if (!SCPI_ParamUInt32(context, &image_size, TRUE)) {return SCPI_RES_ERR;}

	memcpy((void *) &scpi_amu_dev->transfer_reg[AMU_EXT_PAYLOAD], &image_size, sizeof(image_size));

	if (scpi_cmd_extended(context, CMD_EXT_FIRMWARE_BEGIN, AMU_EXT_PAYLOAD + sizeof(image_size), sizeof(uint32_t)) != SCPI_RES_OK) {
		return SCPI_RES_ERR;
	}

	// Answers only once the destination slot has been erased, so the reply doubles
	// as the host's signal that flash is ready
	SCPI_ResultUInt32Base(context, transfer_read_uint32_t(), 10);
	return SCPI_RES_OK;
}

scpi_result_t scpi_cmd_firmware_data(scpi_t *context) {
	const char *block;
	size_t len;

	if (!SCPI_ParamArbitraryBlock(context, &block, &len, TRUE)) {return SCPI_RES_ERR;}

	if ((len + AMU_OTA_CHUNK_OFFSET) > AMU_TRANSFER_REG_SIZE) {
		SCPI_ErrorPush(context, SCPI_ERROR_ILLEGAL_PARAMETER_VALUE);
		return SCPI_RES_ERR;
	}

	// The device is not told how much the master wrote, so the chunk carries its
	// own length ahead of the data
	uint16_t chunk_len = (uint16_t) len;
	memcpy((void *) &scpi_amu_dev->transfer_reg[AMU_EXT_PAYLOAD], &chunk_len, sizeof(chunk_len));
	memcpy((void *) &scpi_amu_dev->transfer_reg[AMU_OTA_CHUNK_OFFSET], block, len);

	if (scpi_cmd_extended(context, CMD_EXT_FIRMWARE_DATA, AMU_EXT_PAYLOAD + sizeof(chunk_len) + len, sizeof(uint32_t)) != SCPI_RES_OK) {
		return SCPI_RES_ERR;
	}

	SCPI_ResultUInt32Base(context, transfer_read_uint32_t(), 10);

	return SCPI_RES_OK;
}

scpi_result_t scpi_cmd_firmware_end(scpi_t *context) {
	if (scpi_cmd_extended(context, CMD_EXT_FIRMWARE_END, AMU_EXT_PAYLOAD, sizeof(int32_t)) != SCPI_RES_OK) {
		return SCPI_RES_ERR;
	}

	SCPI_ResultInt32(context, (int32_t) transfer_read_uint32_t());
	return SCPI_RES_OK;
}

scpi_result_t scpi_cmd_firmware_state(scpi_t *context) {
	amu_fw_status_t status;

	if (scpi_cmd_extended(context, CMD_EXT_FIRMWARE_STATE, AMU_EXT_PAYLOAD, sizeof(status)) != SCPI_RES_OK) {
		return SCPI_RES_ERR;
	}

	memcpy(&status, (const void *) scpi_amu_dev->transfer_reg, sizeof(status));

	SCPI_ResultUInt8(context, status.state);
	SCPI_ResultUInt32Base(context, status.written, 10);
	SCPI_ResultUInt32Base(context, status.total, 10);
	SCPI_ResultUInt32Base(context, status.chunk, 10);

	return SCPI_RES_OK;
}

scpi_result_t scpi_cmd_firmware_commit(scpi_t *context) {
	// A device never commits itself: whoever pushed the update confirms it after
	// checking the device came back, or the next reset rolls it back
	if (scpi_cmd_extended(context, CMD_EXT_FIRMWARE_COMMIT, AMU_EXT_PAYLOAD, context->query ? sizeof(uint8_t) : 0) != SCPI_RES_OK) {
		return SCPI_RES_ERR;
	}

	if (context->query) {
		SCPI_ResultUInt8(context, scpi_amu_dev->transfer_reg[0]);
	}

	return SCPI_RES_OK;
}

scpi_result_t scpi_cmd_firmware_abort(scpi_t *context) {
	return scpi_cmd_extended(context, CMD_EXT_FIRMWARE_ABORT, AMU_EXT_PAYLOAD, 0);
}

/* ---------- SCPI Transport Interface ---------- */

static void scpi_write_str(scpi_t *context, const char* str) {
	context->interface->write(context, str, strlen(str));
}

static size_t SCPI_Write(scpi_t *context, const char* data, size_t len) {
	(void) context;
	return scpi_amu_dev->scpi_dev.write_cmd(data, len);
}

static scpi_result_t SCPI_Write_Control(scpi_t *context, scpi_ctrl_name_t ctrl, scpi_reg_val_t val) {
	char buffer[32];

	if (ctrl == 1) {

#if defined(HAVE_SNPRINTF)
		snprintf(buffer, sizeof(buffer), "**SRQ: 0x%X", val);
		scpi_write_str(context, buffer);
#else
		scpi_write_str(context, "**SRQ: 0x");
		itoa(val, buffer, 16);
		scpi_write_str(context, buffer);
#endif
		scpi_write_str(context, SCPI_LINE_ENDING);
	} else {

#if defined(HAVE_SNPRINTF)
		snprintf(buffer, sizeof(buffer), "**CTRL: 0x%X: 0x%X", ctrl, val);
		scpi_write_str(context, buffer);
#else
		scpi_write_str(context, "**CTRL: 0x");
		itoa(ctrl, buffer, 16);
		scpi_write_str(context, buffer);
		scpi_write_str(context, ": 0x");
		itoa(val, buffer, 16);
		scpi_write_str(context, buffer);
#endif
		scpi_write_str(context, SCPI_LINE_ENDING);
	}

	return SCPI_RES_OK;
}

static scpi_result_t SCPI_Reset(scpi_t *context) {
	if (scpi_amu_dev->scpi_dev.reset_cmd) {
		scpi_amu_dev->scpi_dev.reset_cmd();
	}

	return SCPI_RES_OK;
}

static scpi_result_t SCPI_Flush(scpi_t *context) {
	if (scpi_amu_dev->scpi_dev.flush_cmd) {
		scpi_amu_dev->scpi_dev.flush_cmd();
	}

	return SCPI_RES_OK;
}

/* ---------- SCPI Command Table ---------- */

// This is difficult to read, but necessary in order to place SCPI strings in program memory, otherwise, we use up ~3K of SRAM or ~40%
// First SCPI_COMMAND def applies above, (SCPI_COMMANDS hasn't been called yet, so we can define it after)
// The first call creates program memory char[] pointers for every command as a variable which looks like i.e. scpi_amu_heater_CMD_WRITE_HEATER_PID
// After defining all the areas, we redefine SCPI_COMMAND to place these arrays into the scpi_commands[] array, with the corresponding function and tag calls

#ifndef SCPI_COMMAND
#ifdef SCPI_USE_PROGMEM
	#define SCPI_COMMAND(P, C, T) static const char C ## _ ## T ## _pattern[] PROGMEM = P;
#else
	#define SCPI_COMMAND(P, C, T) static const char C ## _ ## T ## _pattern[] = P;
#endif
#endif


#ifdef __AMU_LOW_MEMORY__
	__AMU_DEFAULT_CMD_LIST__
#else
	__AMU_DEFAULT_CMD_LIST__
	__AMU_EXTENDED_CMD_LIST__
#endif

#undef SCPI_COMMAND

#define SCPI_COMMAND(P, C, T) {C ## _ ## T ## _pattern, C, T},
#ifdef __AMU_SCPI_USE_PROGMEM__
	static const scpi_command_t scpi_def_commands[] PROGMEM = {
#else
	static const scpi_command_t scpi_def_commands[] = {
#endif
	
#ifdef __AMU_LOW_MEMORY__
	__AMU_DEFAULT_CMD_LIST__
	SCPI_CMD_LIST_END
#else
	__AMU_DEFAULT_CMD_LIST__
	__AMU_EXTENDED_CMD_LIST__
	SCPI_CMD_LIST_END
#endif
};

const scpi_command_t* amu_scpi_get_command_list(void) {return scpi_def_commands;}

#undef SCPI_COMMAND

/* ---------- SCPI Public API ---------- */

void amu_scpi_init(volatile amu_device_t* dev, const char* idn1, const char* idn2, const char* idn3, const char* idn4) {
	scpi_amu_dev = dev;

	scpi_interface.error = NULL;
	scpi_interface.write = SCPI_Write;
	scpi_interface.control = SCPI_Write_Control;
	scpi_interface.flush = SCPI_Flush;
	scpi_interface.reset = SCPI_Reset;

	SCPI_Init(&scpi_context,
		scpi_def_commands,
		&scpi_interface,
		scpi_units_def,
		idn1,
		idn2,
		idn3,
		idn4,
		scpi_input_buffer, AMULIBC_SCPI_INPUT_BUFFER_LENGTH,
		scpi_error_queue_data, AMULIBC_SCPI_ERROR_QUEUE_SIZE);
}

void amu_scpi_update(const char incomingByte) {
	SCPI_Input(&scpi_context, &incomingByte, sizeof(char));
}

void amu_scpi_update_buffer(const char* buffer, size_t len) {
	SCPI_Input(&scpi_context, buffer, len);
}

void amu_scpi_add_aux_commands(const scpi_command_t* aux_cmd_list) {
	scpi_context.aux_cmdlist = aux_cmd_list;
}

static void scpi_write_cmd_patterns(const scpi_command_t* cmd_list) {
	if (cmd_list == NULL) {
		return;
	}

#ifdef __AMU_SCPI_USE_PROGMEM__
	char cmd_pattern[SCPI_MAX_CMD_PATTERN_SIZE];
	PGM_P pattern;

	for (int32_t i = 0; (pattern = (PGM_P) pgm_read_word(&(cmd_list[i].pattern))) != 0; i++) {
		strncpy_P(cmd_pattern, pattern, SCPI_MAX_CMD_PATTERN_SIZE);
		scpi_write_str(&scpi_context, cmd_pattern);
	}
#else
	char* cmd_pattern;

	for (int32_t i = 0; (cmd_pattern = (char *) cmd_list[i].pattern) != 0; i++) {
		scpi_write_str(&scpi_context, cmd_pattern);
		scpi_write_str(&scpi_context, ",");
	}
#endif
}

void amu_scpi_list_commands(void) {
	scpi_write_cmd_patterns(scpi_context.def_cmdlist);
	scpi_write_cmd_patterns(scpi_context.aux_cmdlist);

	scpi_write_str(&scpi_context, SCPI_LINE_ENDING);
	SCPI_Flush(&scpi_context);
}

#endif // __AMU_USE_SCPI__
