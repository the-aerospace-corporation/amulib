/**
 * @file amu_device.c
 * @brief TODO
 *
 * @author	CJM28241
 * @date	5/7/2019 3:56:47 PM
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
static uint16_t transfer_reg_data_len = 0;

#ifdef __AMU_DEVICE__
static uint8_t amu_num_devices = 1;


char dev_deviceType_str[AMU_DEVICE_STR_LEN] = AMU_DEVICE_DEFAULT_STR;
char dev_manufacturer_str[AMU_MANUFACTURER_STR_LEN] = AMU_MANUFACTURER_DEFAULT_STR;
char dev_serialNumber_str[AMU_SERIALNUM_STR_LEN] = AMU_SERIALNUM_DEFAULT_STR;
char dev_firmware_str[AMU_FIRMWARE_STR_LEN] = AMU_FIRMWARE_DEFAULT_STR;


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

/**
 * @brief TODO
 *
 * @param amu_device_t Any AMU device
 */
volatile amu_device_t* amu_dev_init(amu_transfer_fptr_t transfer_ptr) {

	if (amu_dev_initialized > 0)
		return &amu_device;

	else {

		

		amu_device.transfer = transfer_ptr;

#ifdef __AMU_DEVICE__
		amu_device_addresses[0] = AMU_THIS_DEVICE;
		amu_device.amu_regs = amu_regs_get_twi_regs_ptr();
#else
		amu_device_addresses[0] = AMU_NO_ADDRESS_MATCH;
#endif
		

#ifdef __AMU_USE_SCPI__
		amu_scpi_init(&amu_device, dev_deviceType_str, dev_manufacturer_str, dev_serialNumber_str, dev_firmware_str);
#endif

		amu_dev_initialized = 1;

		return &amu_device;
	}
}

/**
 * @brief Transfering... TODO
 *
 * @param address 	TODO
 * @param reg 		TODO
 * @param data 		TODO
 * @param len 		TODO
 * @param rw 		TODO
 * @return int8_t
 */
int8_t amu_dev_transfer(uint8_t address, uint8_t reg, uint8_t* data, size_t len, uint8_t rw) {
	return amu_device.transfer(address, reg, data, len, rw);
}

/**
 * @brief Checks to see if the AMU device is busy
 *
 * @param address 	TODO
 * @return uint8_t 	1 is busy, 0 otherwise
 */
uint8_t amu_dev_busy(uint8_t address) {
// #ifdef __AMU_DEVICE__
// 	amu_dev_transfer(address, (uint8_t)AMU_REG_CMD, (uint8_t*)&amu_device.amu_regs->twi_status, sizeof(uint8_t), AMU_TWI_TRANSFER_READ);
// 	return amu_device.amu_regs->twi_status;
// #else
	uint8_t cmd;
	amu_dev_transfer(address, (uint8_t)AMU_REG_CMD, &cmd, sizeof(uint8_t), AMU_TWI_TRANSFER_READ);
	return cmd;
// #endif
}

/**
 * @brief Sends a command to an AMU device
 *
 * @param address TODO
 * @param command Command for the device
 * @return int8_t TODO
 */
int8_t amu_dev_send_command(uint8_t address, CMD_t command) {
	return amu_dev_transfer(address, (uint8_t)AMU_REG_CMD, (uint8_t*)&command, 1, AMU_TWI_TRANSFER_WRITE);
}

/**
 * @brief Move local transfer reg to remote transfer reg, then send command
 *
 * @param address 	TODO
 * @param command 	Command for the device
 * @param len 		Length of TODO
 * @return int8_t TODO
 */
int8_t amu_dev_send_command_data(uint8_t address, CMD_t command, uint8_t len) {
	if (len > 0)
		amu_dev_transfer(address, (uint8_t)AMU_REG_TRANSFER_PTR, (uint8_t*)amu_transfer_reg, len, AMU_TWI_TRANSFER_WRITE);
	return amu_dev_send_command(address, command);
}

/**
 * @brief TODO
 *
 * @param address 			TODO
 * @param command 			TODO
 * @param commandDataLen 	TODO
 * @param responseLength 	TODO
 * @return int8_t
 */
int8_t amu_dev_query_command(uint8_t address, CMD_t command, uint8_t commandDataLen, uint8_t responseLength) {
	
	uint8_t repeat = 0;

	amu_dev_send_command_data(address, (command | CMD_READ), commandDataLen);

	do {
		if (amu_device.delay)
			amu_device.delay(3);
		if (amu_device.watchdog_kick)
			amu_device.watchdog_kick();
		repeat++;
	} while (amu_dev_busy(address) && (repeat < 200));

	if (repeat >= 200)
		return -3;
	else
		return amu_dev_transfer(address, (uint8_t)AMU_REG_TRANSFER_PTR, (uint8_t*)amu_transfer_reg, responseLength, AMU_TWI_TRANSFER_READ);
}

/**
 * @brief Scans for any devices that might be connected in order to identify them
 *
 * @param startAddress 	Start address of the scan range
 * @param endAddress 	End address of the scan range
 * @return uint8_t 	Number of devices found, including this device if __AMU_DEVICE__ is defined
 */
uint8_t amu_scan_for_devices(uint8_t startAddress, uint8_t endAddress) {

#ifdef __AMU_DEVICE__
	amu_num_devices = 1;
	amu_device_addresses[0] = AMU_THIS_DEVICE;
#else
	amu_num_devices = 0;
#endif

	amu_device_addresses[amu_num_devices] = AMU_NO_ADDRESS_MATCH;		//termination

	for (uint8_t i = startAddress; i < endAddress; i++) {

		if (i == AMU_TWI_ALLCALL_ADDRESS)
			continue;
		else {
			if (amu_dev_transfer(i, 0, NULL, 0, AMU_TWI_TRANSFER_READ) == 0) {
				amu_device_addresses[amu_num_devices] = i;
				amu_num_devices = amu_num_devices + 1;
				if (amu_num_devices == AMU_MAX_CONNECTED_DEVICES) {
					break;
				}
			}
		}
	}

	amu_device_addresses[amu_num_devices] = AMU_NO_ADDRESS_MATCH;		//termination

	return amu_num_devices;
}

int8_t amu_get_num_devices() {
	return amu_num_devices;
}

int8_t amu_get_num_connected_devices() {
	return amu_num_devices - 1;
}

/**
 * @brief Device addresses are used for TODO
 *
 * @param deviceNum Device number of interest
 * @return uint8_t Device address
 */
uint8_t amu_get_device_address(uint8_t deviceNum) {
	if (deviceNum < amu_num_devices)
		return amu_device_addresses[deviceNum];
	else
		return AMU_NO_ADDRESS_MATCH;
}

/**
 * @brief TODO
 *
 * TODO
 *
 * @param deviceNum 	TODO
 * @param cmd 			TODO
 * @param transferLen 	TODO
 * @param query 		TODO
 * @return uint8_t TODO
 */
uint8_t _amu_route_command(uint8_t deviceNum, CMD_t cmd, size_t  transferLen, bool query) {

	uint8_t twi_address;

	if (cmd == (CMD_t)CMD_SYSTEM_NO_CMD)
		return 0;

	if ((deviceNum >= amu_num_devices) || (deviceNum == AMU_DEVICE_END_LIST)) {
		memset((uint8_t*)amu_transfer_reg, 0x00, transferLen);			// Clear the transfer reg
		return 0;
	}

	if (deviceNum > 0) {
		if ((twi_address = amu_get_device_address(deviceNum)) != AMU_NO_ADDRESS_MATCH) {

			if (transferLen > AMU_TRANSFER_REG_SIZE) transferLen = AMU_TRANSFER_REG_SIZE;

			if (cmd >= CMD_I2C_USB) {
				if (cmd & CMD_READ) {
					amu_dev_query_command(twi_address, (uint8_t)cmd, 1, transferLen);
				}
				else {
					amu_dev_send_command_data(twi_address, (uint8_t)cmd, transferLen);
				}
			}
			else {
				if (query) {
					memset((void *)amu_transfer_reg, 0x00, transferLen);			//clear transfer reg to zeros before new data is read in.
					amu_dev_transfer(twi_address, (uint8_t)cmd, (uint8_t*)amu_transfer_reg, transferLen, AMU_TWI_TRANSFER_READ);
				}
				else
					amu_dev_transfer(twi_address, (uint8_t)cmd, (uint8_t*)amu_transfer_reg, transferLen, AMU_TWI_TRANSFER_WRITE);
			}
		}
	}
	else {
		if (cmd >= CMD_I2C_USB) {
			if (amu_device.process_cmd != NULL)
				amu_device.process_cmd(cmd);
			else
				amu_command_complete();
		}
		else {
			amu_data_reg_t* amu_register = amu_get_register_ptr(cmd & 0xFF);

			if (query) {
				memcpy((uint8_t*)amu_transfer_reg, (uint8_t*)amu_register, transferLen);
			}
			else {
				memcpy((uint8_t*)amu_register, (uint8_t*)amu_transfer_reg, transferLen);
			}
		}
	}

	return true;
}


/**
 * @brief TODO: explain the transfer part of a transfer read
 *
 * @param offset 	TODO
 * @param data 		TODO
 * @param len 		TODO
 */
void _amu_transfer_read(size_t offset, void* data, size_t len) {
	if ((offset + len) < AMU_TRANSFER_REG_SIZE) {
		memcpy(data, (void*)&amu_transfer_reg[offset], len);
	}
	else
		memset(data, 0, len);
}

/**
 * @brief TODO: explain the transfer part of a transfer write
 *
 * @param offset 	TODO
 * @param data 		TODO
 * @param len 		TODO
 */
void _amu_transfer_write(size_t offset, void* data, size_t len) {
	if ((offset + len) < AMU_TRANSFER_REG_SIZE) {
		memcpy((void*)&amu_transfer_reg[offset], data, len);

		transfer_reg_data_len = offset + len; // Update the length of data in the transfer register
	}
	
}

volatile uint8_t* amu_dev_get_transfer_reg_ptr(void) { return amu_transfer_reg; }
	
amu_data_reg_t* amu_get_register_ptr(uint8_t reg) {

    switch(reg) {
        case AMU_REG_SYSTEM_CMD:                        return (amu_data_reg_t*)&amu_device.amu_regs->command;                break;
        case AMU_REG_SYSTEM_AMU_STATUS:                 return (amu_data_reg_t*)&amu_device.amu_regs->amu_status;             break;
        case AMU_REG_SYSTEM_TWI_STATUS:                 return (amu_data_reg_t*)&amu_device.amu_regs->twi_status;             break;
        case AMU_REG_SYSTEM_HARDWARE_REVISION:          return (amu_data_reg_t*)&amu_device.amu_regs->hardware_revision;      break;
        case AMU_REG_SYSTEM_TSENSOR_TYPE:               return (amu_data_reg_t*)&amu_device.amu_regs->tsensor_type;           break;
        case AMU_REG_SYSTEM_TSENSOR_NUM:                return (amu_data_reg_t*)&amu_device.amu_regs->tsensor_num;            break;
        case AMU_REG_SYSTEM_ADC_ACTIVE_CHANNELS:        return (amu_data_reg_t*)&amu_device.amu_regs->activeADCchannels;      break;
        case AMU_REG_SYSTEM_STATUS_HRADC:               return (amu_data_reg_t*)&amu_device.amu_regs->adc_status;             break;

        case AMU_REG_DUT_JUNCTION:                      return (amu_data_reg_t*)&amu_device.amu_regs->dut.junction;           break;
        case AMU_REG_DUT_COVERGLASS:                    return (amu_data_reg_t*)&amu_device.amu_regs->dut.coverglass;         break;
        case AMU_REG_DUT_INTERCONNECT:                  return (amu_data_reg_t*)&amu_device.amu_regs->dut.interconnect;       break;
        case AMU_REG_DUT_RESERVED:                      return (amu_data_reg_t*)&amu_device.amu_regs->dut.reserved;           break;
        case AMU_REG_DUT_MANUFACTURER:                  return (amu_data_reg_t*)&amu_device.amu_regs->dut.manufacturer;       break;
        case AMU_REG_DUT_MODEL:                         return (amu_data_reg_t*)&amu_device.amu_regs->dut.model;              break;
        case AMU_REG_DUT_TECHNOLOGY:                    return (amu_data_reg_t*)&amu_device.amu_regs->dut.technology;         break;
        case AMU_REG_DUT_SERIAL_NUMBER:                 return (amu_data_reg_t*)&amu_device.amu_regs->dut.serial;             break;
        case AMU_REG_DUT_ENERGY:                        return (amu_data_reg_t*)&amu_device.amu_regs->dut.energy;             break;
        case AMU_REG_DUT_DOSE:                          return (amu_data_reg_t*)&amu_device.amu_regs->dut.dose;               break;

        case AMU_REG_ADC_DATA_VOLTAGE:                  return (amu_data_reg_t*)&amu_device.amu_regs->adc_raw.val.voltage;          break;
        case AMU_REG_ADC_DATA_CURRENT:                  return (amu_data_reg_t*)&amu_device.amu_regs->adc_raw.val.current;          break;
        // case AMU_REG_ADC_DATA_TSENSOR:                  return (amu_data_reg_t*)&amu_device.amu_regs->adc_raw.val.tsensors;         break;
        // case AMU_REG_ADC_DATA_TSENSORS:                 return (amu_data_reg_t*)&amu_device.amu_regs->adc_raw.val.tsensors;         break;
        case AMU_REG_ADC_DATA_TSENSOR_0:                return (amu_data_reg_t*)&amu_device.amu_regs->adc_raw.val.tsensors[0];      break;
        case AMU_REG_ADC_DATA_TSENSOR_1:                return (amu_data_reg_t*)&amu_device.amu_regs->adc_raw.val.tsensors[1];      break;
        case AMU_REG_ADC_DATA_TSENSOR_2:                return (amu_data_reg_t*)&amu_device.amu_regs->adc_raw.val.tsensors[2];      break;
        case AMU_REG_ADC_DATA_BIAS:                     return (amu_data_reg_t*)&amu_device.amu_regs->adc_raw.val.bias;              break;
        case AMU_REG_ADC_DATA_OFFSET:                   return (amu_data_reg_t*)&amu_device.amu_regs->adc_raw.val.offset;            break;
        case AMU_REG_ADC_DATA_TEMP:                     return (amu_data_reg_t*)&amu_device.amu_regs->adc_raw.val.adc_temp;          break;
        case AMU_REG_ADC_DATA_AVDD:                     return (amu_data_reg_t*)&amu_device.amu_regs->adc_raw.val.avdd;              break;
        case AMU_REG_ADC_DATA_IOVDD:                    return (amu_data_reg_t*)&amu_device.amu_regs->adc_raw.val.iovdd;             break;
        case AMU_REG_ADC_DATA_ALDO:                     return (amu_data_reg_t*)&amu_device.amu_regs->adc_raw.val.aldo;              break;
        case AMU_REG_ADC_DATA_DLDO:                     return (amu_data_reg_t*)&amu_device.amu_regs->adc_raw.val.dldo;              break;
        case AMU_REG_ADC_DATA_SS_TL:                    return (amu_data_reg_t*)&amu_device.amu_regs->adc_raw.val.ss_tl;             break;
        case AMU_REG_ADC_DATA_SS_BL:                    return (amu_data_reg_t*)&amu_device.amu_regs->adc_raw.val.ss_bl;             break;
        case AMU_REG_ADC_DATA_SS_BR:                    return (amu_data_reg_t*)&amu_device.amu_regs->adc_raw.val.ss_br;             break;
        case AMU_REG_ADC_DATA_SS_TR:                    return (amu_data_reg_t*)&amu_device.amu_regs->adc_raw.val.ss_tr;             break;

        // case AMU_REG_SUNSENSOR_TL:                      return (amu_data_reg_t*)&amu_device.amu_regs->ss_angle.diode[0];          break;
        // case AMU_REG_SUNSENSOR_BL:                      return (amu_data_reg_t*)&amu_device.amu_regs->ss_angle.diode[1];          break;
        // case AMU_REG_SUNSENSOR_BR:                      return (amu_data_reg_t*)&amu_device.amu_regs->ss_angle.diode[2];          break;
        // case AMU_REG_SUNSENSOR_TR:                      return (amu_data_reg_t*)&amu_device.amu_regs->ss_angle.diode[3];          break;
        case AMU_REG_SUNSENSOR_YAW:                     return (amu_data_reg_t*)&amu_device.amu_regs->ss_angle.yaw;                     break;
        case AMU_REG_SUNSENSOR_PITCH:                   return (amu_data_reg_t*)&amu_device.amu_regs->ss_angle.pitch;                   break;

        case AMU_REG_TIME_MILLIS:                       return (amu_data_reg_t*)&amu_device.amu_regs->milliseconds;            break;
        case AMU_REG_TIME_UTC:                          return (amu_data_reg_t*)&amu_device.amu_regs->utc_time;                break;

        case AMU_REG_SWEEP_CONFIG_TYPE:                return (amu_data_reg_t*)&amu_device.amu_regs->sweep_config.type;              	break;
        case AMU_REG_SWEEP_CONFIG_NUM_POINTS:          return (amu_data_reg_t*)&amu_device.amu_regs->sweep_config.numPoints;        	break;
        case AMU_REG_SWEEP_CONFIG_DELAY:               return (amu_data_reg_t*)&amu_device.amu_regs->sweep_config.delay;             	break;
        case AMU_REG_SWEEP_CONFIG_RATIO:               return (amu_data_reg_t*)&amu_device.amu_regs->sweep_config.ratio;             	break;
        case AMU_REG_SWEEP_CONFIG_PWR_MODE:            return (amu_data_reg_t*)&amu_device.amu_regs->sweep_config.power;             	break;
        case AMU_REG_SWEEP_CONFIG_DAC_GAIN:            return (amu_data_reg_t*)&amu_device.amu_regs->sweep_config.dac_gain;          	break;
        case AMU_REG_SWEEP_CONFIG_AVERAGES:            return (amu_data_reg_t*)&amu_device.amu_regs->sweep_config.sweep_averages;       break;
        case AMU_REG_SWEEP_CONFIG_ADC_AVERAGES:        return (amu_data_reg_t*)&amu_device.amu_regs->sweep_config.adc_averages;         break;
        case AMU_REG_SWEEP_CONFIG_AM0:                 return (amu_data_reg_t*)&amu_device.amu_regs->sweep_config.am0;                 	break;
        case AMU_REG_SWEEP_CONFIG_AREA:                return (amu_data_reg_t*)&amu_device.amu_regs->sweep_config.area;                	break;

        case AMU_REG_SWEEP_META_VOC:                  return (amu_data_reg_t*)&amu_device.amu_regs->meta.voc;                        break;
        case AMU_REG_SWEEP_META_ISC:                  return (amu_data_reg_t*)&amu_device.amu_regs->meta.isc;                        break;
        case AMU_REG_SWEEP_META_TSENSOR_START:        return (amu_data_reg_t*)&amu_device.amu_regs->meta.tsensor_start;              break;
        case AMU_REG_SWEEP_META_TSENSOR_END:          return (amu_data_reg_t*)&amu_device.amu_regs->meta.tsensor_end;                break;
        case AMU_REG_SWEEP_META_FF:                   return (amu_data_reg_t*)&amu_device.amu_regs->meta.ff;                         break;
        case AMU_REG_SWEEP_META_EFF:                  return (amu_data_reg_t*)&amu_device.amu_regs->meta.eff;                        break;
        case AMU_REG_SWEEP_META_VMAX:                 return (amu_data_reg_t*)&amu_device.amu_regs->meta.vmax;                       break;
        case AMU_REG_SWEEP_META_IMAX:                 return (amu_data_reg_t*)&amu_device.amu_regs->meta.imax;                       break;
        case AMU_REG_SWEEP_META_PMAX:                 return (amu_data_reg_t*)&amu_device.amu_regs->meta.pmax;                       break;
        case AMU_REG_SWEEP_META_ADC:                  return (amu_data_reg_t*)&amu_device.amu_regs->meta.adc;                        break;
        case AMU_REG_SWEEP_META_TIMESTAMP:            return (amu_data_reg_t*)&amu_device.amu_regs->meta.timestamp;                 break;
        case AMU_REG_SWEEP_META_CRC:                  return (amu_data_reg_t*)&amu_device.amu_regs->meta.crc;                        break;

        case AMU_REG_DATA_PTR_TIMESTAMP:    return (amu_data_reg_t*)amu_device.sweep_data->timestamp;               break;
        case AMU_REG_DATA_PTR_VOLTAGE:      return (amu_data_reg_t*)amu_device.sweep_data->voltage;                 break;
        case AMU_REG_DATA_PTR_CURRENT:      return (amu_data_reg_t*)amu_device.sweep_data->current;                 break;
        case AMU_REG_DATA_PTR_SS_YAW:       return (amu_data_reg_t*)amu_device.sweep_data->yaw;                     break;
        case AMU_REG_DATA_PTR_SS_PITCH:     return (amu_data_reg_t*)amu_device.sweep_data->pitch;                   break;

        case AMU_REG_DATA_PTR_SWEEP_CONFIG: return (amu_data_reg_t*)&amu_device.amu_regs->sweep_config;                       break;
        case AMU_REG_DATA_PTR_SWEEP_META:   return (amu_data_reg_t*)&amu_device.amu_regs->meta;                               break;
        case AMU_REG_DATA_PTR_SUNSENSOR:    return (amu_data_reg_t*)&amu_device.amu_regs->ss_angle;                           break;
        case AMU_REG_DATA_PTR_PRESSURE:     return (amu_data_reg_t*)&amu_device.amu_regs->adc_raw.val.ss_tl;             break;	// assume request is for quad_photo_sensor_t includeing 4 raw values, not just yaw/pitch
        case AMU_REG_TRANSFER_PTR:          return (amu_data_reg_t*)amu_device.transfer_reg;							break;

		default:                            return NULL;                                                                 break;
    }

}


#ifdef __AMU_DEVICE__

volatile ivsweep_packet_t* amu_dev_get_sweep_packet_ptr(void) { return amu_device.sweep_data; }
volatile amu_twi_regs_t* amu_dev_get_twi_regs_ptr(void) { return amu_regs_get_twi_regs_ptr(); }
volatile amu_scpi_dev_t* amu_get_scpi_dev(void) { return (volatile amu_scpi_dev_t*)&amu_device.scpi_dev; }


char* amu_dev_setDeviceTypeStr(const char* deviceTypeStr) {
	memcpy(dev_deviceType_str, deviceTypeStr, AMU_DEVICE_STR_LEN);
	return dev_deviceType_str;
}

char* amu_dev_setManufacturerStr(const char* manufacturerStr) {
	memcpy(dev_manufacturer_str, manufacturerStr, AMU_MANUFACTURER_STR_LEN);
	return dev_manufacturer_str;
}

char* amu_dev_setSerialNumStr(const char* serialNumStr) {
	memcpy(dev_serialNumber_str, serialNumStr, AMU_SERIALNUM_STR_LEN);
	return dev_serialNumber_str;
}

char* amu_dev_setFirmwareStr(const char* firmwareStr) {
	memcpy(dev_firmware_str, firmwareStr, AMU_FIRMWARE_STR_LEN);
	return dev_firmware_str;
}

uint16_t amu_reg_get_length(uint8_t reg) {
	if(reg == AMU_REG_TRANSFER_PTR) {
		return transfer_reg_data_len;
	}
	else {
		return amu_regs_get_register_length(reg);
	}
	
}

#endif