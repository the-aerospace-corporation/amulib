/**
 * @file amu_device.h
 *
 * @author	CJM28241
 * @date	5/7/2019 3:56:32 PM
 */


#ifndef __AMU_DEVICE_H__
#define __AMU_DEVICE_H__

#include "amu_commands.h"
#include "amu_types.h"

#define AMU_TWI_DEFAULT_ADDRESS			0x0F
#define AMU_TWI_ALLCALL_ADDRESS			0x0A
#define AMU_EYAS_ADDRESS				0x07

#ifdef __AMU_LOW_MEMORY__
#	define AMU_MAX_CONNECTED_DEVICES		16
#else
#	define AMU_MAX_CONNECTED_DEVICES		63
#endif

#define AMU_DEVICE_STR_LEN				10
#define AMU_MANUFACTURER_STR_LEN		20
#define AMU_SERIALNUM_STR_LEN			26
#define AMU_FIRMWARE_STR_LEN			16

#define AMU_DEVICE_DEFAULT_STR			"AMU-M0+"
#define AMU_MANUFACTURER_DEFAULT_STR	"AEROSPACE"
#define AMU_SERIALNUM_DEFAULT_STR		"0x01"
#define AMU_FIRMWARE_DEFAULT_STR		"amu-"

#define AMU_TWI_TRANSFER_READ	1
#define AMU_TWI_TRANSFER_WRITE	0

#ifdef	__cplusplus
extern "C" {
#endif

	extern volatile amu_device_t amu_device;
	extern char dev_deviceType_str[AMU_DEVICE_STR_LEN];
	extern char dev_manufacturer_str[AMU_MANUFACTURER_STR_LEN];
	extern char dev_serialNumber_str[AMU_SERIALNUM_STR_LEN];
	extern char dev_firmware_str[AMU_FIRMWARE_STR_LEN];

	volatile amu_device_t* 		amu_dev_init(amu_transfer_fptr_t);

	int8_t						amu_dev_transfer(uint8_t address, uint8_t reg, uint8_t* data, size_t len, uint8_t rw);
	uint8_t						amu_dev_busy(uint8_t address);

	int8_t						amu_dev_send_command(uint8_t address, CMD_t command);
	int8_t						amu_dev_send_command_data(uint8_t address, CMD_t command, uint8_t len);
	int8_t						amu_dev_query_command(uint8_t address, CMD_t command, uint8_t commandDataLen, uint8_t responseLength);

	static inline CMD_t			amu_get_next_twi_command(void) { return (CMD_t)(amu_device.amu_regs->command + CMD_I2C_USB); }
	static inline void			amu_command_complete(void) { amu_device.amu_regs->command = 0;}

	uint8_t						amu_scan_for_devices(uint8_t startAddress, uint8_t endAddress);
	int8_t						amu_get_num_devices(void);
	int8_t						amu_get_num_connected_devices(void);
	uint8_t						amu_get_device_address(uint8_t deviceNum);

	uint8_t						_amu_route_command(uint8_t deviceNum, CMD_t cmd, size_t transferLen, bool query);

	void						_amu_transfer_read(size_t offset, void *data, size_t len);
	void						_amu_transfer_write(size_t offset, void *data, size_t len);

	volatile uint8_t*			amu_dev_get_transfer_reg_ptr(void);

	amu_data_reg_t*				amu_get_register_ptr(uint8_t amu_register);

	
#ifdef __AMU_DEVICE__
	volatile ivsweep_packet_t*	amu_dev_get_sweep_packet_ptr(void);
	volatile amu_twi_regs_t*	amu_dev_get_twi_regs_ptr(void);
	volatile amu_scpi_dev_t*	amu_get_scpi_dev(void);

	static inline char*			amu_dev_getDeviceTypeStr(void) { return dev_deviceType_str; }
	static inline char*			amu_dev_getManufacturerStr(void) { return dev_manufacturer_str; }
	static inline char*			amu_dev_getSerialNumStr(void) { return dev_serialNumber_str; }
	static inline char*			amu_dev_getFirmwareStr(void) { return dev_firmware_str; }

	char*						amu_dev_setDeviceTypeStr(const char* deviceTypeStr);
	char*						amu_dev_setManufacturerStr(const char* manufacturerStr);
	char*						amu_dev_setSerialNumStr(const char* serialNumStr);
	char*						amu_dev_setFirmwareStr(const char* firmwareStr);

	uint16_t 					amu_reg_get_length(uint8_t reg);


#endif


	
#define __TRANSFER_READ_(TYPE)					static inline TYPE transfer_read_##TYPE(void) { TYPE data; _amu_transfer_read(0, &data, sizeof(TYPE)); return data; }
#define __TRANSFER_READ_OFFSET(TYPE)			static inline TYPE transfer_read_offset_##TYPE(void) { TYPE data; _amu_transfer_read(1, &data, sizeof(TYPE)); return data; }
#define __TRANSFER_WRITE_(TYPE)					static inline void transfer_write_##TYPE(TYPE data)  { _amu_transfer_write(0, &data, sizeof(TYPE)); }

	__TRANSFER_READ_(uint8_t)
	__TRANSFER_READ_(uint16_t)
	__TRANSFER_READ_(uint32_t)
	__TRANSFER_READ_(int32_t)
	__TRANSFER_READ_(float)
	__TRANSFER_READ_(amu_coeff_t)
	__TRANSFER_READ_(amu_pid_t)
	__TRANSFER_READ_(press_data_t)	//these weren't here before...
	__TRANSFER_READ_(ss_angle_t)
	__TRANSFER_READ_(amu_meas_t)

	__TRANSFER_READ_OFFSET(uint8_t)
	__TRANSFER_READ_OFFSET(uint16_t)
	__TRANSFER_READ_OFFSET(uint32_t)
	__TRANSFER_READ_OFFSET(int32_t)
	__TRANSFER_READ_OFFSET(float)

	__TRANSFER_WRITE_(uint8_t)
	__TRANSFER_WRITE_(uint16_t)
	__TRANSFER_WRITE_(uint32_t)
	__TRANSFER_WRITE_(int32_t)
	__TRANSFER_WRITE_(float)
	__TRANSFER_WRITE_(amu_coeff_t)
	__TRANSFER_WRITE_(amu_pid_t)
	__TRANSFER_WRITE_(press_data_t)
	__TRANSFER_WRITE_(ss_angle_t)

	static inline void transfer_write_str(const char* str, uint8_t len) { _amu_transfer_write(0, (void*)str, len); }
	static inline void transfer_read_str(const char* str, uint8_t len) { _amu_transfer_read(0, (void*)str, len); }



#ifdef	__cplusplus
}
#endif

#endif /* __AMU_DEVICE_H__ */