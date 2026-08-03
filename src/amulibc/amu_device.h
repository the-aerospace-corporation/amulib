/**
 * @file amu_device.h
 * @brief AMU device management, command dispatch, and typed TWI transfer helpers
 *
 * @author CJM28241
 * @date 5/7/2019
 */
#ifndef __AMU_DEVICE_H__
#define __AMU_DEVICE_H__

#include "amu_commands.h"
#include "amu_types.h"
#include "amu_config_internal.h"
#include "amulib_version.h"

#define AMU_TWI_DEFAULT_ADDRESS			0x0F
#define AMU_TWI_ALLCALL_ADDRESS			0x0A
#define AMU3_TWI_ALLCALL_ADDRESS        0x00
#define AMU_EYAS_ADDRESS				0x07

#ifndef AMU_TWI_FREQ_HZ
#define AMU_TWI_FREQ_HZ					400000UL
#endif

#define AMU_DEV_WAIT_TIMEOUT_MS			25000
#define AMU_DEV_WAIT_INITIAL_DELAY_MS	1
#define AMU_DEV_WAIT_MAX_DELAY_MS		100

#ifdef __AMU_LOW_MEMORY__
#define AMU_MAX_CONNECTED_DEVICES		16
#else
#define AMU_MAX_CONNECTED_DEVICES		63
#endif

#define AMU_DEVICE_STR_LEN				10
#define AMU_MANUFACTURER_STR_LEN		20
#define AMU_SERIALNUM_STR_LEN			26
#define AMU_FIRMWARE_STR_LEN			16
#define AMU_AMULIB_STR_LEN				32
typedef char amulib_version_len_exceeds_max[(AMULIB_VERSION_STR_LEN > AMU_AMULIB_STR_LEN) ? -1 : 1];

#define AMU_DEVICE_DEFAULT_STR			"AMU-M0+"
#define AMU_MANUFACTURER_DEFAULT_STR	"AEROSPACE"
#define AMU_SERIALNUM_DEFAULT_STR		"0x01"
#define AMU_FIRMWARE_DEFAULT_STR		"amu-"

#define AMU_TWI_TRANSFER_READ			1
#define AMU_TWI_TRANSFER_WRITE			0

#define AMU_TRANSFER_REG_SENTINEL		0xFF

#ifdef	__cplusplus
extern "C" {
#endif
	extern volatile amu_device_t amu_device;

	extern char dev_deviceType_str[AMU_DEVICE_STR_LEN];
	extern char dev_manufacturer_str[AMU_MANUFACTURER_STR_LEN];
	extern char dev_serialNumber_str[AMU_SERIALNUM_STR_LEN];
	extern char dev_firmware_str[AMU_FIRMWARE_STR_LEN];
	extern const char dev_amulib_str[AMU_AMULIB_STR_LEN];

	volatile amu_device_t* 		amu_dev_init(amu_transfer_fptr_t);
	int8_t						amu_dev_transfer(uint8_t address, uint8_t reg, uint8_t* data, size_t len, uint8_t rw);
	/** @brief 0 ready, 1 busy, negative if the device could not be reached */
	int8_t						amu_dev_busy(uint8_t address);

	int8_t						amu_dev_send_command(uint8_t address, CMD_t command);
	int8_t						amu_dev_send_command_data(uint8_t address, CMD_t command, size_t len);
	int8_t 						amu_dev_wait_until_ready(uint8_t address, uint32_t timeout_ms, uint32_t initial_delay_ms, uint32_t max_delay_ms);
	int8_t						amu_dev_query_command(uint8_t address, CMD_t command, size_t commandDataLen, size_t responseLength);
	int8_t						amu_dev_collect_result(uint8_t address, size_t responseLength);
	uint8_t						_amu_route_command(uint8_t deviceNum, CMD_t cmd, size_t transferLen, bool query);

	uint8_t						_amu_route_command_data(uint8_t deviceNum, CMD_t cmd, size_t inLen, size_t transferLen, bool query);
	uint8_t						_amu_route_trigger(uint8_t deviceNum, CMD_t cmd, size_t inLen);
	uint8_t						_amu_route_collect(uint8_t deviceNum, CMD_t cmd, size_t transferLen);

	static inline CMD_t			amu_get_next_twi_command(void) { return (CMD_t)(amu_device.amu_regs->command + CMD_I2C_USB); }
	static inline void			amu_command_complete(void) { amu_device.amu_regs->command = 0; }

	uint8_t						amu_scan_for_devices(uint8_t startAddress, uint8_t endAddress);
	int8_t						amu_get_num_devices(void);
	int8_t						amu_get_num_connected_devices(void);
	uint8_t						amu_get_device_address(uint8_t deviceNum);

	void						_amu_transfer_read(size_t offset, void *data, size_t len);
	void						_amu_transfer_write(size_t offset, void *data, size_t len);
	void						_amu_transfer_reset_len(void);
	void						_amu_transfer_clear(size_t len);

	volatile uint8_t*			amu_dev_get_transfer_reg_ptr(void);
	amu_data_reg_t*				amu_get_register_ptr(uint8_t amu_register);
	uint16_t 					amu_reg_get_length(uint8_t reg);

#ifdef __AMU_USE_SCPI__
	volatile amu_scpi_dev_t*	amu_get_scpi_dev(void);
	amu_scpi_dev_t*				amu_scpi_dev_init(size_t(*write_cmd)(const char* data, size_t len), void(*flush_cmd)(void));
#endif // __AMU_USE_SCPI__


#ifdef __AMU_DEVICE__
	volatile ivsweep_packet_t*	amu_dev_get_sweep_packet_ptr(void);
	volatile amu_twi_regs_t*	amu_dev_get_twi_regs_ptr(void);

	static inline char*			amu_dev_getDeviceTypeStr(void) { return dev_deviceType_str; }
	static inline char*			amu_dev_getManufacturerStr(void) { return dev_manufacturer_str; }
	static inline char*			amu_dev_getSerialNumStr(void) { return dev_serialNumber_str; }
	static inline char*			amu_dev_getFirmwareStr(void) { return dev_firmware_str; }
	static inline const char*	amu_dev_getAmulibStr(void) { return dev_amulib_str; }

	char*						amu_dev_setDeviceTypeStr(const char* deviceTypeStr);
	char*						amu_dev_setManufacturerStr(const char* manufacturerStr);
	char*						amu_dev_setSerialNumStr(const char* serialNumStr);
	char*						amu_dev_setFirmwareStr(const char* firmwareStr);
#endif
	
#define __TRANSFER_READ_(TYPE)					static inline TYPE transfer_read_##TYPE(void) { TYPE data; _amu_transfer_read(0, &data, sizeof(TYPE)); return data; }
#define __TRANSFER_READ_OFFSET(TYPE)			static inline TYPE transfer_read_offset_##TYPE(void) { TYPE data; _amu_transfer_read(1, &data, sizeof(TYPE)); return data; }

#define __TRANSFER_READ_EXT(TYPE)				static inline TYPE transfer_read_ext_##TYPE(void) { TYPE data; _amu_transfer_read(AMU_EXT_PAYLOAD, &data, sizeof(TYPE)); return data; }

#define __TRANSFER_WRITE_(TYPE)					static inline void transfer_write_##TYPE(TYPE data)  { _amu_transfer_write(0, &data, sizeof(TYPE)); }
#define __TRANSFER_WRITE_OFFSET(TYPE)			static inline void transfer_write_offset_##TYPE(uint8_t num, TYPE data)  { _amu_transfer_write(num*sizeof(TYPE), &data, sizeof(TYPE)); }

	__TRANSFER_READ_(uint8_t)
	__TRANSFER_READ_(uint16_t)
	__TRANSFER_READ_(uint32_t)
	__TRANSFER_READ_(int32_t)
	__TRANSFER_READ_(float)
	__TRANSFER_READ_(amu_coeff_t)
	__TRANSFER_READ_(amu_pid_t)
	__TRANSFER_READ_(press_data_t)
	__TRANSFER_READ_(ss_angle_t)
	__TRANSFER_READ_(amu_meas_t)

	__TRANSFER_READ_OFFSET(uint8_t)
	__TRANSFER_READ_OFFSET(uint16_t)
	__TRANSFER_READ_OFFSET(uint32_t)
	__TRANSFER_READ_OFFSET(int32_t)
	__TRANSFER_READ_OFFSET(float)

	__TRANSFER_READ_EXT(uint8_t)
	__TRANSFER_READ_EXT(uint16_t)
	__TRANSFER_READ_EXT(uint32_t)
	__TRANSFER_READ_EXT(int32_t)
	__TRANSFER_READ_EXT(float)

	__TRANSFER_WRITE_(uint8_t)
	__TRANSFER_WRITE_(uint16_t)
	__TRANSFER_WRITE_(uint32_t)
	__TRANSFER_WRITE_(int32_t)
	__TRANSFER_WRITE_(float)
	__TRANSFER_WRITE_(amu_coeff_t)
	__TRANSFER_WRITE_(amu_pid_t)
	__TRANSFER_WRITE_(press_data_t)
	__TRANSFER_WRITE_(ss_angle_t)
	__TRANSFER_WRITE_(quad_photo_sensor_t)
	__TRANSFER_WRITE_(amu_meas_t)

	__TRANSFER_WRITE_OFFSET(float)

	static inline void transfer_write_str(const char* str, uint8_t len) { _amu_transfer_write(0, (void*) str, len); }
	static inline void transfer_read_str(char* str, uint8_t len) { _amu_transfer_read(0, str, len); }
#ifdef	__cplusplus
}
#endif

#endif /* __AMU_DEVICE_H__ */