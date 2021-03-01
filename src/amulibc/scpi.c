/**
 * @file scpi.c
 * @brief TODO
 *
 * @author	CJM28241
 * @date	5/7/2019 7:16:32 PM
 */

#include "scpi.h"
#include "amu_device.h"
#include "amu_regs.h"
#include "amu_config_internal.h"
#include <stdio.h>

static uint8_t scpi_channel_list[65];
static volatile amu_device_t* scpi_amu_dev;

#ifdef __AMU_USE_SCPI__
static char scpi_input_buffer[SCPI_INPUT_BUFFER_LENGTH];
static scpi_error_t scpi_error_queue_data[SCPI_ERROR_QUEUE_SIZE];
#endif

static scpi_t scpi_context;
static scpi_interface_t scpi_interface;

static size_t o_count = 1;

#define SCPI_Param_amu_pid_t(c, v, b)		SCPI_ParamArrayFloat(c, v, 3, &o_count, SCPI_FORMAT_ASCII, b)
#define SCPI_Result_amu_pid_t(c, v)			SCPI_ResultArrayFloat(c, (float *)&(v), 3, SCPI_FORMAT_ASCII)

#define SCPI_Param_amu_coeff_t(c, v, b)		SCPI_ParamArrayFloat(c, v, 4, &o_count, SCPI_FORMAT_ASCII, b)
#define SCPI_Result_amu_coeff_t(c, v)		SCPI_ResultArrayFloat(c, (float *)&(v), 4, SCPI_FORMAT_ASCII)

#define SCPI_Param_amu_notes_t(c, v, b)		SCPI_ParamCopyText(c, v, AMU_NOTES_SIZE, &o_count, b)
#define SCPI_Result_amu_notes_t(c, v)		SCPI_ResultText(c, (char *)v)

#define SCPI_Param_ss_angle_t(c, v, b)		SCPI_ParamArrayFloat(c, v, 6, &o_count, SCPI_FORMAT_ASCII, b)
#define SCPI_Result_ss_angle_t(c, v)		SCPI_ResultArrayFloat(c, (float *)&(v), 6, SCPI_FORMAT_ASCII)

#define SCPI_Param_press_data_t(c, v, b)	SCPI_ParamArrayFloat(c, v, 4, &o_count, SCPI_FORMAT_ASCII, b)
#define SCPI_Result_press_data_t(c, v)		SCPI_ResultArrayFloat(c, (float *)&(v), 4, SCPI_FORMAT_ASCII)

#define SCPI_Param_amu_int_volt_t(c, v, b)	SCPI_ParamArrayFloat(c, v, 4, &o_count, SCPI_FORMAT_ASCII, b)
#define SCPI_Result_amu_int_volt_t(c, v)	SCPI_ResultArrayFloat(c, (float *)&(v), 4, SCPI_FORMAT_ASCII)

#define SCPI_Param_amu_meas_t(c, v, b)		SCPI_ParamArrayFloat(c, v, 2, &o_count, SCPI_FORMAT_ASCII, b)
#define SCPI_Result_amu_meas_t(c, v)		SCPI_ResultArrayFloat(c, (float *)&(v), 2, SCPI_FORMAT_ASCII)

amu_notes_t* notes_ptr;


#define SCPI_CMD_RW(TYPE)																							\
scpi_result_t scpi_cmd_rw_##TYPE(scpi_t *context) {																	\
																													\
	int32_t channel = -1;																							\
																													\
	memset((void *)scpi_amu_dev->transfer_reg, 0, sizeof(TYPE));													\
																													\
	SCPI_CommandNumbers(context, &channel, 1, -1);																	\
																													\
	scpi_amu_dev->transfer_reg[0] = channel;																		\
																													\
	if(!context->query) {																							\
		if(channel >= 0) {																							\
			if( !SCPI_Param_##TYPE(context, (void *)&scpi_amu_dev->transfer_reg[1], TRUE)) return SCPI_RES_ERR;		\
		}																											\
		else {																										\
			channel = 0;																							\
			if( !SCPI_Param_##TYPE(context, (void *)scpi_amu_dev->transfer_reg, TRUE)) return SCPI_RES_ERR;			\
		}																											\
	}																												\
	else																											\
		channel = 0;																								\
																													\
	_scpi_get_channelList(context);																					\
																													\
	for(uint8_t *device = scpi_channel_list; *device != AMU_DEVICE_END_LIST; device++) {							\
		if(context->query)	{																						\
			if( SCPI_CmdTag(context) >= CMD_I2C_USB )																\
				_amu_route_command(*device, (SCPI_CmdTag(context) | CMD_READ), sizeof(TYPE), true);					\
			else																									\
				_amu_route_command(*device, SCPI_CmdTag(context), sizeof(TYPE), true);								\
																													\
			TYPE *data = (TYPE *)scpi_amu_dev->transfer_reg;														\
			SCPI_Result_##TYPE(context, *data);																		\
		}																											\
		else {																										\
			_amu_route_command(*device, SCPI_CmdTag(context), sizeof(TYPE), false);									\
		}																											\
	}																												\
																													\
	return SCPI_RES_OK;																								\
}																													\

SCPI_CMD_RW(uint8_t)
SCPI_CMD_RW(uint16_t)
SCPI_CMD_RW(uint32_t)
SCPI_CMD_RW(float)
SCPI_CMD_RW(amu_pid_t)
SCPI_CMD_RW(amu_coeff_t)
SCPI_CMD_RW(amu_notes_t)
SCPI_CMD_RW(ss_angle_t)
SCPI_CMD_RW(press_data_t)
SCPI_CMD_RW(amu_int_volt_t)
SCPI_CMD_RW(amu_meas_t)

#define SCPI_CMD_EXEC_QRY(TYPE)																						\
scpi_result_t scpi_cmd_exec_qry_##TYPE(scpi_t *context) {															\
																													\
	int32_t channel = -1;																							\
																													\
	memset((void *)scpi_amu_dev->transfer_reg, 0, sizeof(TYPE));													\
																													\
	SCPI_CommandNumbers(context, &channel, 1, -1);																	\
																													\
	scpi_amu_dev->transfer_reg[0] = channel;																		\
																													\
	_scpi_get_channelList(context);																					\
																													\
	for(uint8_t *device = scpi_channel_list; *device != AMU_DEVICE_END_LIST; device++) {							\
		if(context->query)	{																						\
			if( SCPI_CmdTag(context) >= CMD_I2C_USB )																\
				_amu_route_command(*device, (SCPI_CmdTag(context) | CMD_READ), sizeof(TYPE), true);					\
			else																									\
				_amu_route_command(*device, SCPI_CmdTag(context), sizeof(TYPE), true);								\
																													\
			TYPE *data = (TYPE *)scpi_amu_dev->transfer_reg;														\
			SCPI_Result_##TYPE(context, *data);																		\
		}																											\
		else {																										\
			if( channel == -1)																						\
				_amu_route_command(*device, SCPI_CmdTag(context), 0, false);										\
			else																									\
				_amu_route_command(*device, SCPI_CmdTag(context), 1, false);										\
		}																											\
	}																												\
																													\
	return SCPI_RES_OK;																								\
}																													\

SCPI_CMD_EXEC_QRY(uint8_t)
SCPI_CMD_EXEC_QRY(uint16_t)
SCPI_CMD_EXEC_QRY(uint32_t)
SCPI_CMD_EXEC_QRY(float)

// If commmand number exists, it's placed in the first byte of the transfer register
// NO parameters, otherwise error is thrown.
scpi_result_t scpi_cmd_execute(scpi_t* context) {

	int32_t* commandNumber = (int32_t*)scpi_amu_dev->transfer_reg;

	SCPI_CommandNumbers(context, commandNumber, 1, -1);

	_scpi_get_channelList(context);

	for (uint8_t* device = scpi_channel_list; *device != AMU_DEVICE_END_LIST; device++) {
		if (*commandNumber == -1)
			_amu_route_command(*device, SCPI_CmdTag(context), 0, false);
		else
			_amu_route_command(*device, SCPI_CmdTag(context), 1, false);
	}
	return SCPI_RES_OK;
}

scpi_result_t _scpi_read_ptr(scpi_t* context) {

	uint8_t numPoints;

	_scpi_get_channelList(context);

	for (uint8_t* device = scpi_channel_list; *device != AMU_DEVICE_END_LIST; device++) {

		_amu_route_command(*device, AMU_REG_SWEEP_CONFIG_NUM_POINTS, 1, true);
		numPoints = scpi_amu_dev->transfer_reg[0];

		switch ((AMU_REG_DATA_PTR_t)SCPI_CmdTag(context)) {
		case AMU_REG_DATA_PTR_TIMESTAMP:
			_amu_route_command(*device, SCPI_CmdTag(context), numPoints * sizeof(uint32_t), true);
			SCPI_ResultArrayUInt32(context, (uint32_t*)&scpi_amu_dev->transfer_reg[0], numPoints, SCPI_FORMAT_ASCII);
			break;
		case AMU_REG_DATA_PTR_VOLTAGE:
		case AMU_REG_DATA_PTR_CURRENT:
		case AMU_REG_DATA_PTR_SS_YAW:
		case AMU_REG_DATA_PTR_SS_PITCH:
			_amu_route_command(*device, SCPI_CmdTag(context), numPoints * sizeof(float), true);
			SCPI_ResultArrayFloat(context, (float*)&scpi_amu_dev->transfer_reg[0], numPoints, SCPI_FORMAT_ASCII);
			break;
		case AMU_REG_DATA_PTR_SWEEP_CONFIG:
			_amu_route_command(*device, SCPI_CmdTag(context), sizeof(ivsweep_config_t), true);
			SCPI_ResultArrayUInt8(context, (uint8_t*)&scpi_amu_dev->transfer_reg[0], 8, SCPI_FORMAT_ASCII);
			SCPI_ResultArrayFloat(context, (float*)&scpi_amu_dev->transfer_reg[8], 2, SCPI_FORMAT_ASCII);
			break;
		case AMU_REG_DATA_PTR_SWEEP_META:
			_amu_route_command(*device, SCPI_CmdTag(context), sizeof(ivsweep_meta_t), true);
			SCPI_ResultArrayFloat(context, (float*)&scpi_amu_dev->transfer_reg[0], 10, SCPI_FORMAT_ASCII);
			SCPI_ResultArrayUInt32(context, (uint32_t*)&scpi_amu_dev->transfer_reg[40], 2, SCPI_FORMAT_ASCII);
			break;
		case AMU_REG_DATA_PTR_SUNSENSOR:
			_amu_route_command(*device, SCPI_CmdTag(context), sizeof(ss_angle_t), true);
			SCPI_ResultArrayFloat(context, (float*)&scpi_amu_dev->transfer_reg[0], 2, SCPI_FORMAT_ASCII);
			break;
		case AMU_REG_DATA_PTR_PRESSURE:
			_amu_route_command(*device, SCPI_CmdTag(context), sizeof(press_data_t), true);
			SCPI_ResultArrayFloat(context, (float*)&scpi_amu_dev->transfer_reg[0], 4, SCPI_FORMAT_ASCII);
			break;
		default: break;
		}

	}

	return SCPI_RES_OK;
}

scpi_result_t _scpi_write_sweep_ptr(scpi_t* context) {

	if (!SCPI_ParamArrayFloat(context, (void*)scpi_amu_dev->transfer_reg, AMU_TRANSFER_REG_SIZE / sizeof(float), &o_count, SCPI_FORMAT_ASCII, TRUE)) return SCPI_RES_ERR;

	_scpi_get_channelList(context);

	for (uint8_t* device = scpi_channel_list; *device != AMU_DEVICE_END_LIST; device++) {
		if (o_count <= IVSWEEP_MAX_POINTS)
			_amu_route_command(*device, SCPI_CmdTag(context), (o_count * sizeof(float)), false);
		else
			return SCPI_RES_ERR;
	}

	return SCPI_RES_OK;
}

scpi_result_t _scpi_write_config_ptr(scpi_t* context) {

	uint32_t sweepSettingsUint32_t[8];
	uint8_t* sweepSettings = (uint8_t*)scpi_amu_dev->transfer_reg;
	float* am0 = (float*)&scpi_amu_dev->transfer_reg[8];
	float* area = (float*)&scpi_amu_dev->transfer_reg[12];

	if (!SCPI_ParamArrayUInt32(context, sweepSettingsUint32_t, 8, &o_count, SCPI_FORMAT_ASCII, TRUE)) return SCPI_RES_ERR;
	if (!SCPI_ParamFloat(context, am0, TRUE)) return SCPI_RES_ERR;
	if (!SCPI_ParamFloat(context, area, TRUE)) return SCPI_RES_ERR;

	_scpi_get_channelList(context);

	//Convert uint32_t values to uint8_t and move them into the transfer register
	for (uint8_t i = 0; i < 8; i++) {
		sweepSettings[i] = (uint8_t)sweepSettingsUint32_t[i];
	}

	for (uint8_t* device = scpi_channel_list; *device != AMU_DEVICE_END_LIST; device++) {
		_amu_route_command(*device, SCPI_CmdTag(context), sizeof(ivsweep_config_t), context->query);
	}

	return SCPI_RES_OK;
}

scpi_result_t _scpi_write_meta_ptr(scpi_t* context) {

	if (!SCPI_ParamArrayFloat(context, (void*)scpi_amu_dev->transfer_reg, 9, &o_count, SCPI_FORMAT_ASCII, TRUE)) return SCPI_RES_ERR;
	if (!SCPI_ParamUInt32(context, (void*)&scpi_amu_dev->transfer_reg[36], TRUE)) return SCPI_RES_ERR;

	_scpi_get_channelList(context);

	for (uint8_t* device = scpi_channel_list; *device != AMU_DEVICE_END_LIST; device++) {
		_amu_route_command(*device, SCPI_CmdTag(context), sizeof(ivsweep_meta_t), context->query);
	}

	return SCPI_RES_OK;
}

scpi_result_t _scpi_cmd_led(scpi_t* context) {

	if (!SCPI_ParamUInt32(context, (void*)scpi_amu_dev->transfer_reg, TRUE)) return SCPI_RES_ERR;

	_scpi_get_channelList(context);

	for (uint8_t* device = scpi_channel_list; *device != AMU_DEVICE_END_LIST; device++) {
		_amu_route_command(*device, (CMD_t)(SCPI_CmdTag(context) + scpi_amu_dev->transfer_reg[0]), sizeof(uint8_t), false);
	}

	return SCPI_RES_OK;
}

scpi_result_t _scpi_cmd_measure_channel(scpi_t* context) {

	int32_t channel = -1;

	memset((void*)scpi_amu_dev->transfer_reg, 0, 4);

	SCPI_CommandNumbers(context, &channel, 1, -1);

	if (channel == -1)
		channel = SCPI_CmdTag(context) & 0x0F;		// So instead, use the tag
	else
		scpi_amu_dev->transfer_reg[0] = channel;

	if (channel > 15)
		return SCPI_RES_ERR;

	if (!context->query) {
		return SCPI_RES_ERR;
	}

	_scpi_get_channelList(context);

	for (uint8_t* device = scpi_channel_list; *device != AMU_DEVICE_END_LIST; device++) {

		if (SCPI_CmdTag(context) >= CMD_I2C_USB)
			_amu_route_command(*device, (SCPI_CmdTag(context) | CMD_READ), 4, true);
		else
			_amu_route_command(*device, SCPI_CmdTag(context), 4, true);


		if (strstr(context->param_list.cmd_raw.data, "RAW"))
			SCPI_ResultUInt32Base(context, scpi_amu_dev->amu_regs->adc_raw.channel[channel], 16);
		else
			SCPI_ResultFloat(context, transfer_read_float());
	}

	return SCPI_RES_OK;
}

scpi_result_t _scpi_cmd_measure_active_ch(scpi_t* context) {

	_scpi_get_channelList(context);					// Get the list of channels to act on

	uint16_t* activeChannels = (uint16_t*)scpi_amu_dev->transfer_reg;
	uint8_t numChannels = 0;

	for (uint8_t* device = scpi_channel_list; *device != AMU_DEVICE_END_LIST; device++) {

		_amu_route_command(*device, AMU_REG_SYSTEM_ADC_ACTIVE_CHANNELS, sizeof(uint16_t), true);

		for (uint16_t i = 0; i < 16; i++) {
			if (*activeChannels & (1 << i))
				numChannels++;
		}

		_amu_route_command(*device, SCPI_CmdTag(context), numChannels * 4, false);

		if (numChannels > 0) {
			SCPI_ResultArrayFloat(context, (float*)scpi_amu_dev->transfer_reg, numChannels, SCPI_FORMAT_ASCII);
		}
	}

	return SCPI_RES_OK;
}

scpi_result_t _scpi_cmd_measure_tsensors(scpi_t* context) {

	_scpi_get_channelList(context);					// Get the list of channels to act on

	for (uint8_t* device = scpi_channel_list; *device != AMU_DEVICE_END_LIST; device++) {

		_amu_route_command(*device, SCPI_CmdTag(context), 12, true);

		SCPI_ResultArrayFloat(context, (float*)scpi_amu_dev->transfer_reg, 3, SCPI_FORMAT_ASCII);
	}

	return SCPI_RES_OK;
}

scpi_result_t _scpi_cmd_query_str(scpi_t* context) {

	if (!context->query) {
		if (!SCPI_ParamCopyText(context, (void*)scpi_amu_dev->transfer_reg, AMU_TRANSFER_REG_SIZE, &o_count, TRUE))	return SCPI_RES_ERR;
	}

	_scpi_get_channelList(context);					// Get the list of channels to act on

	for (uint8_t* device = scpi_channel_list; *device != AMU_DEVICE_END_LIST; device++) {

		if (context->query) {
			if (SCPI_CmdTag(context) >= CMD_I2C_USB) {
				switch (SCPI_CmdTag(context)) {
				case CMD_SYSTEM_FIRMWARE:			_amu_route_command(*device, (SCPI_CmdTag(context) | CMD_READ), SCPI_IDN_STRING_LENGTH, true);									break;
				case CMD_SYSTEM_SERIAL_NUM:			_amu_route_command(*device, (SCPI_CmdTag(context) | CMD_READ), SCPI_IDN_STRING_LENGTH, true);									break;
				case CMD_DUT_MANUFACTURER:			_amu_route_command(*device, (SCPI_CmdTag(context) | CMD_READ), sizeof(scpi_amu_dev->amu_regs->dut.manufacturer), true);			break;
				case CMD_DUT_MODEL:					_amu_route_command(*device, (SCPI_CmdTag(context) | CMD_READ), sizeof(scpi_amu_dev->amu_regs->dut.model), true);				break;
				case CMD_DUT_TECHNOLOGY:			_amu_route_command(*device, (SCPI_CmdTag(context) | CMD_READ), sizeof(scpi_amu_dev->amu_regs->dut.technology), true);			break;
				case CMD_DUT_SERIAL_NUMBER:			_amu_route_command(*device, (SCPI_CmdTag(context) | CMD_READ), sizeof(scpi_amu_dev->amu_regs->dut.serial), true);				break;
				case CMD_DUT_NOTES:					_amu_route_command(*device, (SCPI_CmdTag(context) | CMD_READ), AMU_NOTES_SIZE, true);											break;
				default:							_amu_route_command(*device, (SCPI_CmdTag(context) | CMD_READ), AMU_TRANSFER_REG_SIZE, true);									break;
				}
			}
			else {
				switch (SCPI_CmdTag(context)) {
				default:							_amu_route_command(*device, (SCPI_CmdTag(context)), sizeof(scpi_amu_dev->transfer_reg), true);									break;
				case AMU_REG_DUT_MANUFACTURER:		_amu_route_command(*device, (SCPI_CmdTag(context)), sizeof(scpi_amu_dev->amu_regs->dut.manufacturer), true);					break;
				case AMU_REG_DUT_MODEL:				_amu_route_command(*device, (SCPI_CmdTag(context)), sizeof(scpi_amu_dev->amu_regs->dut.model), true);							break;
				case AMU_REG_DUT_TECHNOLOGY:		_amu_route_command(*device, (SCPI_CmdTag(context)), sizeof(scpi_amu_dev->amu_regs->dut.technology), true);						break;
				case AMU_REG_DUT_SERIAL_NUMBER:		_amu_route_command(*device, (SCPI_CmdTag(context)), sizeof(scpi_amu_dev->amu_regs->dut.serial), true);							break;
				}
			}

			SCPI_ResultText(context, (const char*)scpi_amu_dev->transfer_reg);
		}
		else {
			if (SCPI_CmdTag(context) >= CMD_I2C_USB) {
				switch (SCPI_CmdTag(context)) {
				case CMD_SYSTEM_FIRMWARE:		_amu_route_command(*device, (SCPI_CmdTag(context)), SCPI_IDN_STRING_LENGTH, false);													break;
				case CMD_SYSTEM_SERIAL_NUM:		_amu_route_command(*device, (SCPI_CmdTag(context)), SCPI_IDN_STRING_LENGTH, false);													break;
				case CMD_DUT_MANUFACTURER:		_amu_route_command(*device, (SCPI_CmdTag(context)), sizeof(scpi_amu_dev->amu_regs->dut.manufacturer), false);						break;
				case CMD_DUT_MODEL:				_amu_route_command(*device, (SCPI_CmdTag(context)), sizeof(scpi_amu_dev->amu_regs->dut.model), false);								break;
				case CMD_DUT_TECHNOLOGY:		_amu_route_command(*device, (SCPI_CmdTag(context)), sizeof(scpi_amu_dev->amu_regs->dut.technology), false);							break;
				case CMD_DUT_SERIAL_NUMBER:		_amu_route_command(*device, (SCPI_CmdTag(context)), sizeof(scpi_amu_dev->amu_regs->dut.serial), false);								break;
				case CMD_DUT_NOTES:				_amu_route_command(*device, (SCPI_CmdTag(context)), AMU_NOTES_SIZE, false);															break;
				default:						_amu_route_command(*device, (SCPI_CmdTag(context)), AMU_TRANSFER_REG_SIZE, false);													break;
				}
			}
		}
	}

	return SCPI_RES_OK;
}

scpi_result_t _scpi_cmd_twi_scan(scpi_t* context) {

	int32_t* commandNumber = (int32_t*)scpi_amu_dev->transfer_reg;

	SCPI_CommandNumbers(context, commandNumber, 1, -1);

	_scpi_get_channelList(context);

	for (uint8_t* device = scpi_channel_list; *device != AMU_DEVICE_END_LIST; device++) {
		if (*device == AMU_THIS_DEVICE) {
			_amu_route_command(*device, SCPI_CmdTag(context), sizeof(uint8_t), context->query);
			SCPI_ResultInt8(context, amu_get_num_devices());
			for (uint8_t i = 0; i < amu_get_num_devices(); i++) {
				if (i == AMU_THIS_DEVICE)
					SCPI_ResultUInt8(context, scpi_amu_dev->twi_address);
				else
					SCPI_ResultUInt8(context, amu_get_device_address(i));
			}
		}
	}
	return SCPI_RES_OK;
}

static size_t SCPI_Write(scpi_t* context, const char* data, size_t len) {
	(void)context;
	return scpi_amu_dev->scpi_dev.write_cmd(data, len);
}

static scpi_result_t SCPI_Write_Control(scpi_t* context, scpi_ctrl_name_t ctrl, scpi_reg_val_t val) {
	(void)context;

	if (ctrl == 1) {
		printf("**SRQ: 0x%X (%d)\r\n", val, val);
	}
	else {
		printf("**CTRL %02x: 0x%X (%d)\r\n", ctrl, val, val);
	}

	return SCPI_RES_OK;
}

static scpi_result_t SCPI_Reset(scpi_t* context) {
	if (scpi_amu_dev->scpi_dev.reset_cmd)
		scpi_amu_dev->scpi_dev.reset_cmd();
	return SCPI_RES_OK;
}

static scpi_result_t SCPI_Flush(scpi_t* context) {
	if (scpi_amu_dev->scpi_dev.flush_cmd)
		scpi_amu_dev->scpi_dev.flush_cmd();
	return SCPI_RES_OK;
}


// This is difficult to read, but necessary in order to place SCPI strings in program memory, otherwise, we use up ~3K of SRAM or ~40%
// First SCPI_COMMAND def applies above, (SCPI_COMMANDS hasn't been called yet, so we can define it after)
// The first call creates program memory char[] pointers for every command as a variable which looks like i.e. scpi_amu_heater_CMD_WRITE_HEATER_PID
// After defining all the areas, we redefine SCPI_COMMAND to place these arrays into the scpi_commands[] array, with the corresponding function and tag calls

__AMU_DEFAULT_CMD_LIST__
#undef SCPI_COMMAND

#define SCPI_COMMAND(P, C, T) {C ## _ ## T ## _pattern, C, T},
#ifdef SCPI_USE_PROGMEM
static const scpi_command_t scpi_def_commands[] PROGMEM = {
#else
static const scpi_command_t scpi_def_commands[] = {
#endif
	__AMU_DEFAULT_CMD_LIST__
	SCPI_CMD_LIST_END
};
#undef SCPI_COMMAND

void amu_scpi_init(volatile amu_device_t* dev, const char* idn1, const char* idn2, const char* idn3, const char* idn4) {

	scpi_amu_dev = dev;

	scpi_interface.error = NULL;
	scpi_interface.write = SCPI_Write;
	scpi_interface.control = SCPI_Write_Control;
	scpi_interface.flush = SCPI_Flush;
	scpi_interface.reset = SCPI_Reset;

#ifdef __AMU_USE_SCPI__
	SCPI_Init(&scpi_context,
		scpi_def_commands,
		&scpi_interface,
		scpi_units_def,
		idn1,
		idn2,
		idn3,
		idn4,
		scpi_input_buffer, SCPI_INPUT_BUFFER_LENGTH,
		scpi_error_queue_data, SCPI_ERROR_QUEUE_SIZE);
#endif
}

void amu_scpi_update(const char incomingByte) {
	SCPI_Input(&scpi_context, &incomingByte, 1);
}

void amu_scpi_add_aux_commands(const scpi_command_t* aux_cmd_list) {
	scpi_context.aux_cmdlist = aux_cmd_list;
}

void amu_scpi_list_commands(void) {

#ifdef __AMU_USE_SCPI__

	int32_t i;

#ifdef SCPI_USE_PROGMEM

	char cmd_pattern[SCPI_MAX_CMD_PATTERN_SIZE];

	PGM_P pattern = (PGM_P)pgm_read_word(&(scpi_context.def_cmdlist[0].pattern));
	strncpy_P(cmd_pattern, pattern, SCPI_MAX_CMD_PATTERN_SIZE);
	printf("%s", cmd_pattern);

	for (i = 1; (pattern = (PGM_P)pgm_read_word(&(scpi_context.def_cmdlist[i].pattern))) != 0; i++) {
		strncpy_P(cmd_pattern, pattern, SCPI_MAX_CMD_PATTERN_SIZE);
		printf(",%s", cmd_pattern);
	}

	if (scpi_context.aux_cmdlist != NULL) {
		PGM_P pattern = (PGM_P)pgm_read_word(&(scpi_context.aux_cmdlist[0].pattern));
		strncpy_P(cmd_pattern, pattern, SCPI_MAX_CMD_PATTERN_SIZE);
		printf("%s", cmd_pattern);

		for (i = 1; (pattern = (PGM_P)pgm_read_word(&(scpi_context.aux_cmdlist[i].pattern))) != 0; i++) {
			strncpy_P(cmd_pattern, pattern, SCPI_MAX_CMD_PATTERN_SIZE);
			printf(",%s", cmd_pattern);
		}
	}

	#else

	char* cmd_pattern;

	for (i = 0; (cmd_pattern = (char*)scpi_context.def_cmdlist[i].pattern) != 0; i++) {
		printf(",%s", cmd_pattern);
	}

	for (i = 0; (cmd_pattern = (char*)scpi_context.aux_cmdlist[i].pattern) != 0; i++) {
		printf(",%s", cmd_pattern);
	}

	#endif

#endif

	printf(SCPI_LINE_ENDING);

}



int16_t _scpi_get_channelList(scpi_t* context) {

	scpi_parameter_t channel_list_param;
	uint8_t scpi_list_iterator = 0;

	//Checks for any parameter
	if (SCPI_Parameter(context, &channel_list_param, FALSE)) {

		scpi_bool_t is_range;
		size_t dimensions;

		/* the next statement is valid usage and it gets only real number of dimensions for the first item (index 0) */
		if (!SCPI_ExprChannelListEntry(context, &channel_list_param, 0, &is_range, NULL, NULL, 0, &dimensions)) {

			size_t param_idx = 0;			/* index for channel list */
			int8_t direction = 1; /* direction of counter for rows, +/-1 */
			int32_t address_start = 0;
			int32_t address_end = 0;


			while (SCPI_EXPR_OK == SCPI_ExprChannelListEntry(context, &channel_list_param, param_idx, &is_range, &address_start, &address_end, 1, &dimensions)) {

				if ((dimensions != 1) | (address_start > 63)) {
					scpi_channel_list[scpi_list_iterator] = AMU_DEVICE_END_LIST;
					return FALSE;
				}

				if (is_range) {

					if (address_end > 63) {
						scpi_channel_list[scpi_list_iterator] = AMU_DEVICE_END_LIST;
						return FALSE;
					}

					for ((address_start > address_end) ? (direction = -1) : (direction = 1); address_start != address_end; address_start += direction)
					scpi_channel_list[scpi_list_iterator++] = address_start;

					scpi_channel_list[scpi_list_iterator++] = address_start;

				}
				else {
					scpi_channel_list[scpi_list_iterator++] = address_start;
				}

				/* increase index */
				param_idx++;
			}
		}
	}
	else {	//No parameter list
		scpi_channel_list[scpi_list_iterator++] = 0;
	}

	scpi_channel_list[scpi_list_iterator] = AMU_DEVICE_END_LIST;

	return TRUE;
}
