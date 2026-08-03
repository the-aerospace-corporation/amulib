#include <amulib.h>

#include <Arduino.h>
#include <Wire.h>

// 0x00-0x07 are reserved by the I2C spec
#define AMU_DEV_TWI_SCAN_START_ADDRESS 0x08
#define AMU_DEV_TWI_SCAN_END_ADDRESS   0x20

#if defined(ESP32)
	// Replace with your board's SDA and SCL pins
	#define AMU_DEV_TWI_SDA_PIN 35
	#define AMU_DEV_TWI_SCL_PIN 37
#endif

// ----------------------------

amu_device_t *amu_dev = nullptr;

uint8_t amu_process_commands(uint16_t cmd);

void setup() {
	Serial.begin(AMU_BAUD_RATE);
	while (!Serial);

	// Board-specific I2C bus configuration. The ESP32 branch below matches the
	// TinyS3 dev board; on other cores, add your core's pin/buffer/timeout
	// setup here (or nothing - begin() with the default pins also works).
#if defined(ESP32)
	Wire.setPins(AMU_DEV_TWI_SDA_PIN, AMU_DEV_TWI_SCL_PIN);
	Wire.setBufferSize(1024);
	Wire.setTimeOut(1000);
#endif
	Wire.begin();
	Wire.setClock(AMU_TWI_FREQ_HZ);

	amu_dev = AMU::devInit();
	amu_scpi_dev_init(AMU::usbWrite, AMU::usbFlush);
	amu_dev_setDeviceTypeStr("AMU-DEV");
	amu_dev->process_cmd = &amu_process_commands;

	while(Serial.available()) {Serial.read();}
}

void loop() {
	AMU::scpiProcessStream(Serial);
}

uint8_t amu_process_commands(uint16_t cmd) {
	uint16_t root = AMU_GET_CMD_ROOT(cmd);
	uint8_t branch = AMU_GET_CMD_BRANCH(cmd);

	amu_dev->amu_regs->command = (uint8_t) cmd;

	switch(root) {
		case CMD_SYSTEM: switch((CMD_SYSTEM_t) (root | branch)) {
			case CMD_SYSTEM_NO_CMD:             break;
			case CMD_SYSTEM_TWI_NUM_DEVICES:    if (cmd & CMD_READ) transfer_write_uint8_t(amu_get_num_devices()); break;
			case CMD_SYSTEM_SERIAL_NUM:         if (cmd & CMD_READ) transfer_write_str(amu_dev_getSerialNumStr(), AMU_SERIALNUM_STR_LEN); break;
			default: memset((void *) amu_dev_get_transfer_reg_ptr(), 0, AMU_TRANSFER_REG_SIZE); break;
		} break;

		case CMD_USB_SYSTEM: switch((CMD_USB_SYSTEM_t) (root | branch)) {
			case CMD_USB_SYSTEM_TWI_SCAN:        if (cmd & CMD_READ) amu_scan_for_devices(AMU_DEV_TWI_SCAN_START_ADDRESS, AMU_DEV_TWI_SCAN_END_ADDRESS); break;
			default: memset((void *) amu_dev_get_transfer_reg_ptr(), 0, AMU_TRANSFER_REG_SIZE); break;
		} break;

		default: memset((void *) amu_dev_get_transfer_reg_ptr(), 0, AMU_TRANSFER_REG_SIZE);
	}

	amu_command_complete();

	return 0;
}