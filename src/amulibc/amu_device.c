/**
 * @file amu_device.c
 * @brief AMU device management and TWI transfer implementation
 *
 * @author	CJM28241
 * @date 5/7/2019 
 */
#include "amu_config_internal.h"
#pragma message(AMULIBC_CONFIG_MESSAGE)

#include "amu_device.h"
#include "amu_types.h"
#include "amu_regs.h"
#include "amu_commands.h"

#ifdef __AMU_USE_SCPI__
#include "scpi.h"
#endif

static uint8_t amu_dev_initialized = 0;

static uint8_t amu_device_addresses[AMU_MAX_CONNECTED_DEVICES];

static volatile uint8_t amu_transfer_reg[AMU_TRANSFER_REG_SIZE];
static volatile uint16_t transfer_reg_data_len = 0;

static void _amu_transfer_grow_data_len(size_t len);

#ifdef __AMU_DEVICE__
static uint8_t amu_num_devices = 1;

char dev_deviceType_str[AMU_DEVICE_STR_LEN] = AMU_DEVICE_DEFAULT_STR;
char dev_manufacturer_str[AMU_MANUFACTURER_STR_LEN] = AMU_MANUFACTURER_DEFAULT_STR;
char dev_serialNumber_str[AMU_SERIALNUM_STR_LEN] = AMU_SERIALNUM_DEFAULT_STR;
char dev_firmware_str[AMU_FIRMWARE_STR_LEN] = AMU_FIRMWARE_DEFAULT_STR;
const char dev_amulib_str[AMU_AMULIB_STR_LEN] = AMULIB_VERSION_STR;

#else
static uint8_t amu_num_devices = 0;
#endif

volatile amu_device_t amu_device = {
	.transfer_reg = amu_transfer_reg,
	.sweep_data = NULL,
	.amu_regs = NULL,
	.transfer = NULL,
	.delay = NULL,
	.watchdog_kick = NULL,
	.hardware_reset = NULL,
	.millis = NULL,
	.process_cmd = NULL,
};

volatile amu_device_t* amu_dev_init(amu_transfer_fptr_t transfer_ptr) {
	if (amu_dev_initialized > 0) {
		return &amu_device;
	}

	amu_device.transfer = transfer_ptr;

#ifdef __AMU_DEVICE__
	amu_device_addresses[0] = AMU_THIS_DEVICE;
	amu_device.amu_regs = amu_regs_get_twi_regs_ptr();
#else
	amu_device_addresses[0] = AMU_NO_ADDRESS_MATCH;
#endif

#if defined(__AMU_USE_SCPI__) && defined(__AMU_DEVICE__)
	amu_scpi_init(&amu_device, dev_deviceType_str, dev_manufacturer_str, dev_serialNumber_str, dev_firmware_str);
#endif
	amu_dev_initialized = 1;

	return &amu_device;
}

int8_t amu_dev_transfer(uint8_t address, uint8_t reg, uint8_t* data, size_t len, uint8_t rw) {
	return amu_device.transfer(address, reg, data, len, rw);
}

int8_t amu_dev_busy(uint8_t address) {
	uint8_t command = 1;

	// Unreachable
	if (amu_dev_transfer(address, (uint8_t) AMU_REG_CMD, &command, sizeof(amu_device.amu_regs->command), AMU_TWI_TRANSFER_READ) != 0) {
		return -1;
	}

	return (command != 0) ? 1 : 0;
}

int8_t amu_dev_send_command(uint8_t address, CMD_t command) {
	return amu_dev_transfer(address, (uint8_t) AMU_REG_CMD, (uint8_t*) &command, sizeof(amu_device.amu_regs->command), AMU_TWI_TRANSFER_WRITE);
}

int8_t amu_dev_send_command_data(uint8_t address, CMD_t command, size_t len) {
	if (len > 0) {
		int8_t rv = amu_dev_transfer(address, (uint8_t) AMU_REG_TRANSFER_PTR, (uint8_t*) amu_transfer_reg, len, AMU_TWI_TRANSFER_WRITE);
		if (rv != 0) {return rv;}
	}

	return amu_dev_send_command(address, command);
}

int8_t amu_dev_collect_result(uint8_t address, size_t responseLength) {
	int8_t rv = amu_dev_wait_until_ready(address, AMU_DEV_WAIT_TIMEOUT_MS, AMU_DEV_WAIT_INITIAL_DELAY_MS, AMU_DEV_WAIT_MAX_DELAY_MS);
	if (rv != 0) {return rv;}

	return amu_dev_transfer(address, (uint8_t) AMU_REG_TRANSFER_PTR, (uint8_t*) amu_transfer_reg, responseLength, AMU_TWI_TRANSFER_READ);
}

int8_t amu_dev_query_command(uint8_t address, CMD_t command, size_t commandDataLen, size_t responseLength) {
	int8_t rv = amu_dev_send_command_data(address, (command | CMD_READ), commandDataLen);
	if (rv != 0) {return rv;}

	return amu_dev_collect_result(address, responseLength);
}

int8_t amu_dev_wait_until_ready(uint8_t address, uint32_t timeout_ms, uint32_t initial_delay_ms, uint32_t max_delay_ms) {
	uint32_t elapsed_ms = 0;
	uint32_t delay_ms = initial_delay_ms ? initial_delay_ms : 1;

	while (elapsed_ms < timeout_ms) {
		if (amu_dev_busy(address) == 0) {return 0;}

		if (amu_device.delay) {amu_device.delay(delay_ms);}
		if (amu_device.watchdog_kick) {amu_device.watchdog_kick();}

		elapsed_ms += delay_ms;

		// Exponential backoff (Double delay with each iteration)
		delay_ms *= 2;
		if (delay_ms > max_delay_ms) {
			delay_ms = max_delay_ms;
		}
	}

	return -3;
}

uint8_t amu_scan_for_devices(uint8_t startAddress, uint8_t endAddress) {
#ifdef __AMU_DEVICE__
	amu_num_devices = 1;
	amu_device_addresses[0] = AMU_THIS_DEVICE;
#else
	amu_num_devices = 0;
#endif

	for (uint8_t addr = startAddress; addr < endAddress; addr++) {
		if (addr == AMU_TWI_ALLCALL_ADDRESS) {continue;}
		if (addr == AMU3_TWI_ALLCALL_ADDRESS) {continue;}
		if (amu_dev_transfer(addr, 0, NULL, 0, AMU_TWI_TRANSFER_READ) != AMU_TWI_STATUS_OK) {continue;}
		if (amu_num_devices >= AMU_MAX_CONNECTED_DEVICES - 1) {break;} // Reserve last slot for termination

		amu_device_addresses[amu_num_devices] = addr;
		amu_num_devices++;
	}

	amu_device_addresses[amu_num_devices] = AMU_NO_ADDRESS_MATCH; // Termination

	return amu_num_devices;
}

int8_t amu_get_num_devices(void) {
	return amu_num_devices;
}

int8_t amu_get_num_connected_devices() {
#ifdef __AMU_DEVICE__
	return amu_num_devices - 1;	// Slot 0 is AMU_THIS_DEVICE, not a connected device
#else
	return amu_num_devices;		// Remote-device builds list only discovered devices
#endif
}

uint8_t amu_get_device_address(uint8_t deviceNum) {
	if (deviceNum < amu_num_devices) {
		return amu_device_addresses[deviceNum];
	} else {
		return AMU_NO_ADDRESS_MATCH;
	}
}

uint8_t _amu_route_command(uint8_t deviceNum, CMD_t cmd, size_t transferLen, bool query) {
	return _amu_route_command_data(deviceNum, cmd, 1, transferLen, query);
}

uint8_t _amu_route_command_data(uint8_t deviceNum, CMD_t cmd, size_t inLen, size_t transferLen, bool query) {
	if (transferLen > AMU_TRANSFER_REG_SIZE) {transferLen = AMU_TRANSFER_REG_SIZE;}
	if (inLen > AMU_TRANSFER_REG_SIZE) {inLen = AMU_TRANSFER_REG_SIZE;}

	// Reset the tracked transfer len before each command
	_amu_transfer_reset_len();

	if ((deviceNum >= amu_num_devices) || (deviceNum == AMU_DEVICE_END_LIST)) {
		_amu_transfer_clear(transferLen);
		return 0;
	}

	// Remote device: route the command over TWI
	if (deviceNum > 0) {
		uint8_t twi_address = amu_get_device_address(deviceNum);
		if (twi_address == AMU_NO_ADDRESS_MATCH) {return 0;}

		int8_t rv;

		if (AMU_CMD_IS_EXEC(cmd)) {
			if (cmd & CMD_READ) {
				rv = amu_dev_query_command(twi_address, (uint8_t) cmd, inLen, transferLen);
				if (rv == 0) {_amu_transfer_grow_data_len(transferLen);}
			} else {
				rv = amu_dev_send_command_data(twi_address, (uint8_t) cmd, transferLen);
			}
		} else if (query) {
			_amu_transfer_clear(transferLen);
			rv = amu_dev_transfer(twi_address, (uint8_t) cmd, (uint8_t*) amu_transfer_reg, transferLen, AMU_TWI_TRANSFER_READ);
			if (rv == 0) {_amu_transfer_grow_data_len(transferLen);}
		} else {
			rv = amu_dev_transfer(twi_address, (uint8_t) cmd, (uint8_t*) amu_transfer_reg, transferLen, AMU_TWI_TRANSFER_WRITE);
		}

		if (rv != 0) {
			_amu_transfer_reset_len();
			return 0;
		}

		return 1;
	}

	// Device 0 is this device, process locally. 
	if (AMU_CMD_IS_EXEC(cmd)) {
		if (amu_device.process_cmd == NULL) {
			amu_command_complete();
			return 0;
		}

		amu_device.process_cmd(cmd);
		return 1;
	}

	// Local register access
	uint8_t reg = (uint8_t) cmd;
	amu_data_reg_t* amu_register = amu_get_register_ptr(reg);
	size_t amu_reg_length = amu_regs_get_register_length(reg);

	if ((amu_register == NULL) || (amu_reg_length == 0)) {
		_amu_transfer_clear(transferLen);
		return 0;
	}

	if (transferLen > amu_reg_length) {transferLen = amu_reg_length;}

	if (query) {
		memcpy((uint8_t*) amu_transfer_reg, (uint8_t*) amu_register, transferLen);
		_amu_transfer_grow_data_len(transferLen);
	} else {
		memcpy((uint8_t*) amu_register, (uint8_t*) amu_transfer_reg, transferLen);
	}

	return 1;
}

uint8_t _amu_route_trigger(uint8_t deviceNum, CMD_t cmd, size_t inLen) {
	// Remote devices only: device 0 executes inline at collect time -> _amu_route_command
	if ((deviceNum == 0) || (deviceNum == AMU_DEVICE_END_LIST)) {return 0;}

	uint8_t twi_address = amu_get_device_address(deviceNum);
	if (twi_address == AMU_NO_ADDRESS_MATCH) {return 0;}

	return (amu_dev_send_command_data(twi_address, (uint8_t) cmd, inLen) == 0) ? 1 : 0;
}

uint8_t _amu_route_collect(uint8_t deviceNum, CMD_t cmd, size_t transferLen) {
	if (deviceNum == 0) {return 0;}

	if (transferLen > AMU_TRANSFER_REG_SIZE) {transferLen = AMU_TRANSFER_REG_SIZE;}

	_amu_transfer_reset_len();

	if (deviceNum == AMU_DEVICE_END_LIST) {
		_amu_transfer_clear(transferLen);
		return 0;
	}

	uint8_t twi_address = amu_get_device_address(deviceNum);
	if (twi_address == AMU_NO_ADDRESS_MATCH) {return 0;}

	// Already triggered -> wait for it to go idle, then pull the staged result
	if (amu_dev_collect_result(twi_address, transferLen) != 0) {
		_amu_transfer_reset_len();
		return 0;
	}

	_amu_transfer_grow_data_len(transferLen);
	return 1;
}

void _amu_transfer_read(size_t offset, void* data, size_t len) {
	if ((offset + len) <= AMU_TRANSFER_REG_SIZE) {
		memcpy(data, (void*) &amu_transfer_reg[offset], len);
	} else {
		memset(data, 0, len);
	}
}

void _amu_transfer_write(size_t offset, void* data, size_t len) {
	if ((offset + len) <= AMU_TRANSFER_REG_SIZE) {
		memcpy((void*) &amu_transfer_reg[offset], data, len);
		_amu_transfer_grow_data_len(offset + len);
	}
}

static void _amu_transfer_grow_data_len(size_t len) {
	if (len > AMU_TRANSFER_REG_SIZE) {len = AMU_TRANSFER_REG_SIZE;}

	if (len > transfer_reg_data_len) {
		transfer_reg_data_len = (uint16_t) len;
	}
}

void _amu_transfer_reset_len(void) {
	transfer_reg_data_len = 0;
}

void _amu_transfer_clear(size_t len) {
	if (len > AMU_TRANSFER_REG_SIZE) {len = AMU_TRANSFER_REG_SIZE;}
	memset((uint8_t*) amu_transfer_reg, AMU_TRANSFER_REG_SENTINEL, len);
}

volatile uint8_t* amu_dev_get_transfer_reg_ptr(void) { 
	return amu_transfer_reg; 
}
	
amu_data_reg_t* amu_get_register_ptr(uint8_t reg) {
    switch(reg) {
        case AMU_REG_SYSTEM_CMD:                        return (amu_data_reg_t*) &amu_device.amu_regs->command;                break;
        case AMU_REG_SYSTEM_AMU_STATUS:                 return (amu_data_reg_t*) &amu_device.amu_regs->amu_status;             break;
        case AMU_REG_SYSTEM_TWI_STATUS:                 return (amu_data_reg_t*) &amu_device.amu_regs->twi_status;             break;
        case AMU_REG_SYSTEM_HARDWARE_REVISION:          return (amu_data_reg_t*) &amu_device.amu_regs->hardware_revision;      break;
        case AMU_REG_SYSTEM_TSENSOR_TYPE:               return (amu_data_reg_t*) &amu_device.amu_regs->tsensor_type;           break;
        case AMU_REG_SYSTEM_TSENSOR_NUM:                return (amu_data_reg_t*) &amu_device.amu_regs->tsensor_num;            break;
        case AMU_REG_SYSTEM_ADC_ACTIVE_CHANNELS:        return (amu_data_reg_t*) &amu_device.amu_regs->activeADCchannels;      break;
        case AMU_REG_SYSTEM_STATUS_HRADC:               return (amu_data_reg_t*) &amu_device.amu_regs->adc_status;             break;

        case AMU_REG_DUT_JUNCTION:                      return (amu_data_reg_t*) &amu_device.amu_regs->dut.junction;           break;
        case AMU_REG_DUT_COVERGLASS:                    return (amu_data_reg_t*) &amu_device.amu_regs->dut.coverglass;         break;
        case AMU_REG_DUT_INTERCONNECT:                  return (amu_data_reg_t*) &amu_device.amu_regs->dut.interconnect;       break;
        case AMU_REG_DUT_RESERVED:                      return (amu_data_reg_t*) &amu_device.amu_regs->dut.reserved;           break;
        case AMU_REG_DUT_MANUFACTURER:                  return (amu_data_reg_t*) &amu_device.amu_regs->dut.manufacturer;       break;
        case AMU_REG_DUT_MODEL:                         return (amu_data_reg_t*) &amu_device.amu_regs->dut.model;              break;
        case AMU_REG_DUT_TECHNOLOGY:                    return (amu_data_reg_t*) &amu_device.amu_regs->dut.technology;         break;
        case AMU_REG_DUT_SERIAL_NUMBER:                 return (amu_data_reg_t*) &amu_device.amu_regs->dut.serial;             break;
        case AMU_REG_DUT_ENERGY:                        return (amu_data_reg_t*) &amu_device.amu_regs->dut.energy;             break;
        case AMU_REG_DUT_DOSE:                          return (amu_data_reg_t*) &amu_device.amu_regs->dut.dose;               break;

        case AMU_REG_ADC_DATA_VOLTAGE:                  return (amu_data_reg_t*) &amu_device.amu_regs->adc_raw.val.voltage;          break;
        case AMU_REG_ADC_DATA_CURRENT:                  return (amu_data_reg_t*) &amu_device.amu_regs->adc_raw.val.current;          break;
        case AMU_REG_ADC_DATA_TSENSOR_0:                return (amu_data_reg_t*) &amu_device.amu_regs->adc_raw.val.tsensors[0];      break;
        case AMU_REG_ADC_DATA_TSENSOR_1:                return (amu_data_reg_t*) &amu_device.amu_regs->adc_raw.val.tsensors[1];      break;
        case AMU_REG_ADC_DATA_TSENSOR_2:                return (amu_data_reg_t*) &amu_device.amu_regs->adc_raw.val.tsensors[2];      break;
        case AMU_REG_ADC_DATA_BIAS:                     return (amu_data_reg_t*) &amu_device.amu_regs->adc_raw.val.bias;              break;
        case AMU_REG_ADC_DATA_OFFSET:                   return (amu_data_reg_t*) &amu_device.amu_regs->adc_raw.val.offset;            break;
        case AMU_REG_ADC_DATA_TEMP:                     return (amu_data_reg_t*) &amu_device.amu_regs->adc_raw.val.adc_temp;          break;
        case AMU_REG_ADC_DATA_AVDD:                     return (amu_data_reg_t*) &amu_device.amu_regs->adc_raw.val.avdd;              break;
        case AMU_REG_ADC_DATA_IOVDD:                    return (amu_data_reg_t*) &amu_device.amu_regs->adc_raw.val.iovdd;             break;
        case AMU_REG_ADC_DATA_ALDO:                     return (amu_data_reg_t*) &amu_device.amu_regs->adc_raw.val.aldo;              break;
        case AMU_REG_ADC_DATA_DLDO:                     return (amu_data_reg_t*) &amu_device.amu_regs->adc_raw.val.dldo;              break;
        case AMU_REG_ADC_DATA_SS_TL:                    return (amu_data_reg_t*) &amu_device.amu_regs->adc_raw.val.ss_tl;             break;
        case AMU_REG_ADC_DATA_SS_BL:                    return (amu_data_reg_t*) &amu_device.amu_regs->adc_raw.val.ss_bl;             break;
        case AMU_REG_ADC_DATA_SS_BR:                    return (amu_data_reg_t*) &amu_device.amu_regs->adc_raw.val.ss_br;             break;
        case AMU_REG_ADC_DATA_SS_TR:                    return (amu_data_reg_t*) &amu_device.amu_regs->adc_raw.val.ss_tr;             break;

        case AMU_REG_SUNSENSOR_YAW:                     return (amu_data_reg_t*) &amu_device.amu_regs->ss_angle.yaw;                     break;
        case AMU_REG_SUNSENSOR_PITCH:                   return (amu_data_reg_t*) &amu_device.amu_regs->ss_angle.pitch;                   break;

        case AMU_REG_TIME_MILLIS:                       return (amu_data_reg_t*) &amu_device.amu_regs->milliseconds;            break;
        case AMU_REG_TIME_UTC:                          return (amu_data_reg_t*) &amu_device.amu_regs->utc_time;                break;

        case AMU_REG_SWEEP_CONFIG_TYPE:                return (amu_data_reg_t*) &amu_device.amu_regs->sweep_config.type;              	break;
        case AMU_REG_SWEEP_CONFIG_NUM_POINTS:          return (amu_data_reg_t*) &amu_device.amu_regs->sweep_config.numPoints;        	break;
        case AMU_REG_SWEEP_CONFIG_DELAY:               return (amu_data_reg_t*) &amu_device.amu_regs->sweep_config.delay;             	break;
        case AMU_REG_SWEEP_CONFIG_RATIO:               return (amu_data_reg_t*) &amu_device.amu_regs->sweep_config.ratio;             	break;
        case AMU_REG_SWEEP_CONFIG_PWR_MODE:            return (amu_data_reg_t*) &amu_device.amu_regs->sweep_config.power;             	break;
        case AMU_REG_SWEEP_CONFIG_DAC_GAIN:            return (amu_data_reg_t*) &amu_device.amu_regs->sweep_config.dac_gain;          	break;
        case AMU_REG_SWEEP_CONFIG_AVERAGES:            return (amu_data_reg_t*) &amu_device.amu_regs->sweep_config.sweep_averages;       break;
        case AMU_REG_SWEEP_CONFIG_ADC_AVERAGES:        return (amu_data_reg_t*) &amu_device.amu_regs->sweep_config.adc_averages;         break;
        case AMU_REG_SWEEP_CONFIG_AM0:                 return (amu_data_reg_t*) &amu_device.amu_regs->sweep_config.am0;                 	break;
        case AMU_REG_SWEEP_CONFIG_AREA:                return (amu_data_reg_t*) &amu_device.amu_regs->sweep_config.area;                	break;

        case AMU_REG_SWEEP_META_VOC:                  return (amu_data_reg_t*) &amu_device.amu_regs->meta.voc;                        break;
        case AMU_REG_SWEEP_META_ISC:                  return (amu_data_reg_t*) &amu_device.amu_regs->meta.isc;                        break;
        case AMU_REG_SWEEP_META_TSENSOR_START:        return (amu_data_reg_t*) &amu_device.amu_regs->meta.tsensor_start;              break;
        case AMU_REG_SWEEP_META_TSENSOR_END:          return (amu_data_reg_t*) &amu_device.amu_regs->meta.tsensor_end;                break;
        case AMU_REG_SWEEP_META_FF:                   return (amu_data_reg_t*) &amu_device.amu_regs->meta.ff;                         break;
        case AMU_REG_SWEEP_META_EFF:                  return (amu_data_reg_t*) &amu_device.amu_regs->meta.eff;                        break;
        case AMU_REG_SWEEP_META_VMAX:                 return (amu_data_reg_t*) &amu_device.amu_regs->meta.vmax;                       break;
        case AMU_REG_SWEEP_META_IMAX:                 return (amu_data_reg_t*) &amu_device.amu_regs->meta.imax;                       break;
        case AMU_REG_SWEEP_META_PMAX:                 return (amu_data_reg_t*) &amu_device.amu_regs->meta.pmax;                       break;
        case AMU_REG_SWEEP_META_ADC:                  return (amu_data_reg_t*) &amu_device.amu_regs->meta.adc;                        break;
        case AMU_REG_SWEEP_META_TIMESTAMP:            return (amu_data_reg_t*) &amu_device.amu_regs->meta.timestamp;                 break;
        case AMU_REG_SWEEP_META_CRC:                  return (amu_data_reg_t*) &amu_device.amu_regs->meta.crc;                        break;

        case AMU_REG_DATA_PTR_TIMESTAMP:    return amu_device.sweep_data ? (amu_data_reg_t*) amu_device.sweep_data->timestamp : NULL;   break;
        case AMU_REG_DATA_PTR_VOLTAGE:      return amu_device.sweep_data ? (amu_data_reg_t*) amu_device.sweep_data->voltage : NULL;     break;
        case AMU_REG_DATA_PTR_CURRENT:      return amu_device.sweep_data ? (amu_data_reg_t*) amu_device.sweep_data->current : NULL;     break;
#ifndef __AMU_LOW_MEMORY__
        case AMU_REG_DATA_PTR_SS_YAW:       return amu_device.sweep_data ? (amu_data_reg_t*) amu_device.sweep_data->yaw : NULL;         break;
        case AMU_REG_DATA_PTR_SS_PITCH:     return amu_device.sweep_data ? (amu_data_reg_t*) amu_device.sweep_data->pitch : NULL;       break;
#endif
        case AMU_REG_DATA_PTR_SWEEP_CONFIG: return (amu_data_reg_t*) &amu_device.amu_regs->sweep_config;                       break;
        case AMU_REG_DATA_PTR_SWEEP_META:   return (amu_data_reg_t*) &amu_device.amu_regs->meta;                               break;
        case AMU_REG_DATA_PTR_SUNSENSOR:    return (amu_data_reg_t*) &amu_device.amu_regs->ss_angle;                           break;
        case AMU_REG_DATA_PTR_PRESSURE:     return NULL;                                                                 break;
        case AMU_REG_DATA_PTR_DATAPOINT:    return NULL;                                                                 break;
        case AMU_REG_TRANSFER_PTR:          return (amu_data_reg_t*) amu_device.transfer_reg;							break;

		default:                            return NULL;                                                                 break;
    }

}

uint16_t amu_reg_get_length(uint8_t reg) {
	if (reg == AMU_REG_TRANSFER_PTR) {
		return transfer_reg_data_len;
	}

	return amu_regs_get_register_length(reg);
}

#ifdef __AMU_USE_SCPI__
volatile amu_scpi_dev_t* amu_get_scpi_dev(void) {return (volatile amu_scpi_dev_t*) &amu_device.scpi_dev;}

amu_scpi_dev_t* amu_scpi_dev_init(size_t(*write_cmd)(const char* data, size_t len), void(*flush_cmd)(void)) {
	amu_scpi_dev_t* scpi_dev = (amu_scpi_dev_t*) amu_get_scpi_dev();
	if (scpi_dev != NULL) {
		scpi_dev->write_cmd = write_cmd;
		scpi_dev->flush_cmd = flush_cmd;
	}
	return scpi_dev;
}
#endif // __AMU_USE_SCPI__

#ifdef __AMU_DEVICE__

volatile ivsweep_packet_t* amu_dev_get_sweep_packet_ptr(void) {return amu_device.sweep_data;}
volatile amu_twi_regs_t* amu_dev_get_twi_regs_ptr(void) {return amu_regs_get_twi_regs_ptr();}

char* amu_dev_setDeviceTypeStr(const char* deviceTypeStr) {
	memset(dev_deviceType_str, 0, AMU_DEVICE_STR_LEN);
	strncpy(dev_deviceType_str, deviceTypeStr, AMU_DEVICE_STR_LEN - 1);
	return dev_deviceType_str;
}

char* amu_dev_setManufacturerStr(const char* manufacturerStr) {
	memset(dev_manufacturer_str, 0, AMU_MANUFACTURER_STR_LEN);
	strncpy(dev_manufacturer_str, manufacturerStr, AMU_MANUFACTURER_STR_LEN - 1);
	return dev_manufacturer_str;
}

char* amu_dev_setSerialNumStr(const char* serialNumStr) {
	memset(dev_serialNumber_str, 0, AMU_SERIALNUM_STR_LEN);
	strncpy(dev_serialNumber_str, serialNumStr, AMU_SERIALNUM_STR_LEN - 1);
	return dev_serialNumber_str;
}

char* amu_dev_setFirmwareStr(const char* firmwareStr) {
	memset(dev_firmware_str, 0, AMU_FIRMWARE_STR_LEN);
	strncpy(dev_firmware_str, firmwareStr, AMU_FIRMWARE_STR_LEN - 1);
	return dev_firmware_str;
}

#endif
