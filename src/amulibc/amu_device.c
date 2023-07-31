/**
 * @file amu_device.c
 * @brief TODO
 *
 * @author	CJM28241
 * @date	5/7/2019 3:56:47 PM
 */

#include "amu_config_internal.h"
#pragma message(AMU_CONFIG_MESSAGE)

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

#ifdef __AMU_DEVICE__
static uint8_t amu_num_devices = 1;

static volatile amu_twi_regs_t amu_twi_regs;

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
		amu_device.amu_regs = &amu_twi_regs;
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
#ifdef __AMU_DEVICE__
	amu_dev_transfer(address, (uint8_t)AMU_REG_CMD, (uint8_t*)&amu_device.amu_regs->twi_status, sizeof(uint8_t), AMU_TWI_TRANSFER_READ);
	return amu_device.amu_regs->twi_status;
#else
	uint8_t cmd;
	amu_dev_transfer(address, (uint8_t)AMU_REG_CMD, &cmd, sizeof(uint8_t), AMU_TWI_TRANSFER_READ);
	return cmd;
#endif
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
	//if (amu_device.delay)
	//	amu_device.delay(3);
	while (amu_dev_busy(address) && (repeat < 200)) {
		if (amu_device.delay)
			amu_device.delay(5);
		if (amu_device.watchdog_kick)
			amu_device.watchdog_kick();
		repeat++;
	}

	if (repeat >= 200)
		return -3;
	else
		return amu_dev_transfer(address, (uint8_t)AMU_REG_TRANSFER_PTR, (uint8_t*)amu_transfer_reg, responseLength, AMU_TWI_TRANSFER_READ);
}

/**
 * @brief Scans for any devices that might be TODO in order to TODO
 *
 * @param startAddress 	TODO
 * @param endAddress 	TODO
 * @return uint8_t TODO
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
	}
}

volatile uint8_t* amu_dev_get_transfer_reg_ptr(void) { return amu_transfer_reg; }
	
amu_data_reg_t* amu_get_register_ptr(uint8_t amu_register) {

	volatile uint8_t* twi_reg = (volatile uint8_t*)&amu_device.amu_regs->command;

	switch (amu_register) {
		case AMU_REG_DATA_PTR_TIMESTAMP:		return (amu_data_reg_t*)amu_device.sweep_data->timestamp;				break;
		case AMU_REG_DATA_PTR_VOLTAGE:			return (amu_data_reg_t*)amu_device.sweep_data->voltage;					break;
		case AMU_REG_DATA_PTR_CURRENT:			return (amu_data_reg_t*)amu_device.sweep_data->current;					break;
#ifndef __AMU_LOW_MEMORY__
		case AMU_REG_DATA_PTR_SS_YAW:			return (amu_data_reg_t*)amu_device.sweep_data->yaw;						break;
		case AMU_REG_DATA_PTR_SS_PITCH:			return (amu_data_reg_t*)amu_device.sweep_data->pitch;					break;
#else
		case AMU_REG_DATA_PTR_SS_YAW:			return (amu_data_reg_t*)amu_device.sweep_data->voltage;					break;
		case AMU_REG_DATA_PTR_SS_PITCH:			return (amu_data_reg_t*)amu_device.sweep_data->current;					break;
#endif
		case AMU_REG_DATA_PTR_SWEEP_CONFIG:		return (amu_data_reg_t*)&amu_device.amu_regs->sweep_config;				break;
		case AMU_REG_DATA_PTR_SWEEP_META:		return (amu_data_reg_t*)&amu_device.amu_regs->meta;						break;
		case AMU_REG_DATA_PTR_SUNSENSOR:		return (amu_data_reg_t*)&amu_device.amu_regs->ss_angle;					break;
		case AMU_REG_TRANSFER_PTR:				return (amu_data_reg_t*)amu_device.transfer_reg;						break;
		default:								return (amu_data_reg_t*)&twi_reg[amu_register];							break;

	}
}


#ifdef __AMU_DEVICE__

volatile ivsweep_packet_t* amu_dev_get_sweep_packet_ptr(void) { return amu_device.sweep_data; }
volatile amu_twi_regs_t* amu_dev_get_twi_regs_ptr(void) { return &amu_twi_regs; }
volatile amu_scpi_dev_t* amu_get_scpi_dev(void) { return (volatile amu_scpi_dev_t*)&amu_device.scpi_dev; }


char* amu_dev_setDeviceTypeStr(char* deviceTypeStr) {
	memcpy(dev_deviceType_str, deviceTypeStr, AMU_DEVICE_STR_LEN);
	return dev_deviceType_str;
}

char* amu_dev_setManufacturerStr(char* manufacturerStr) {
	memcpy(dev_manufacturer_str, manufacturerStr, AMU_MANUFACTURER_STR_LEN);
	return dev_manufacturer_str;
}

char* amu_dev_setSerialNumStr(char* serialNumStr) {
	memcpy(dev_serialNumber_str, serialNumStr, AMU_SERIALNUM_STR_LEN);
	return dev_serialNumber_str;
}

char* amu_dev_setFirmwareStr(char* firmwareStr) {
	memcpy(dev_firmware_str, firmwareStr, AMU_FIRMWARE_STR_LEN);
	return dev_firmware_str;
}

#endif