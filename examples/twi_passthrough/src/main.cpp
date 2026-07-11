#include <amulib.h>

#include <Arduino.h>
#include <Wire.h>
#include <UMS3.h>
#include <Bounce2.h>

#define AMU_DEV_TWI_BUS Wire
#define AMU_DEV_TWI_SDA_PIN 35
#define AMU_DEV_TWI_SCL_PIN 37

#define AMU_DEV_AMU_RESET_PIN 6
#define AMU_DEV_LOAD_CURRENT_PIN 7

#define AMU_DEV_SW0_PIN 8
#define AMU_DEV_SW1_PIN 9

UMS3 dev_board;
Bounce dev_sw0 = Bounce();
Bounce dev_sw1 = Bounce();

amu_device_t *amu_dev = nullptr;

#define BACKSPACE_CHAR       '\b'
#define DELETE_CHAR          0x7F
#define BACKSPACE_SEQ        "\b \b"
#define NEWLINE_CHAR         '\n'
#define CARRIAGE_RETURN_CHAR '\r'
#define NEWLINE_SEQ          "\r\n"
#define LINE_BUFFER_SIZE AMULIBC_SCPI_INPUT_BUFFER_LENGTH

static char line_buffer[LINE_BUFFER_SIZE];
static size_t line_pos = 0;

void led_color(uint8_t red, uint8_t green, uint8_t blue);
void amu_dut_reset(void);
void amu_dev_scan_for_devices(void);
void amu_dev_check_switch(void);
void amu_dev_read_load_current(void);
void amu_dev_read_usb(void);
int arduino_wire_transfer(TwoWire* wire, uint8_t address, uint8_t reg, uint8_t* data, size_t len, uint8_t read);
int8_t twi_transfer(uint8_t address, uint8_t reg, uint8_t* data, size_t len, uint8_t read);
size_t hal_usb_write(const char* data, size_t len);
void hal_usb_flush(void);
uint8_t amu_process_commands(uint16_t cmd);

void setup() {
	Serial.begin(115200);
	Serial.setTimeout(5000);

	dev_board.begin();
	dev_board.setPixelBrightness(255);

	// Enable the power to the RGB LED.
	// Off by default so it doesn't use current when the LED is not required.
	dev_board.setPixelPower(true);

	AMU_DEV_TWI_BUS.begin(AMU_DEV_TWI_SDA_PIN, AMU_DEV_TWI_SCL_PIN, 400000UL);
	AMU_DEV_TWI_BUS.setBufferSize(1024);
	AMU_DEV_TWI_BUS.setTimeOut(1000); 

	dev_sw0.attach(AMU_DEV_SW0_PIN, INPUT_PULLUP);
	dev_sw1.attach(AMU_DEV_SW1_PIN, INPUT_PULLUP);

	dev_sw0.interval(20);
	dev_sw1.interval(20);

	analogRead(AMU_DEV_LOAD_CURRENT_PIN) / 4095. / 2.00; // Dummy read to stabilize ADC

	amu_dev = (amu_device_t *) amu_dev_init(twi_transfer);
	amu_dev->millis = millis;
	amu_dev->delay = delay;

	amu_dev->scpi_dev.write_cmd = hal_usb_write;
	amu_dev->scpi_dev.flush_cmd = hal_usb_flush;

	amu_dev->process_cmd = &amu_process_commands;

	amu_dev_setDeviceTypeStr("AMU-DEV");

	amu_dev->amu_regs->hardware_revision = AMU_HARDWARE_REVISION_AMU_ESP32_DEV;

	led_color(0, 0, 5); // Blue to indicate ready

	while(Serial.available()) {Serial.read();} // Clear any input buffer
}

void loop() {
	amu_dev_check_switch();
	amu_dev_read_usb();
}

void amu_dut_reset(void) {
	pinMode(AMU_DEV_AMU_RESET_PIN, OUTPUT);
	digitalWrite(AMU_DEV_AMU_RESET_PIN, HIGH);
	delay(100);
	digitalWrite(AMU_DEV_AMU_RESET_PIN, LOW);
	delay(250);
}

void amu_dev_scan_for_devices(void) {
	led_color(0, 0, 0);

	Serial.println("Scanning for devices...");

	int num_dut = amu_scan_for_devices(0x0F, 0x20);

	// Device 0 is this AMU
	if (num_dut > 1) {
		Serial.printf("Found %u AMU(s)\n", num_dut - 1);

		for (uint8_t i = 1; i < num_dut; i++) {
			Serial.printf("Found AMU at address: 0x%02X\n", amu_get_device_address(i));
		}
	} else {
		Serial.println(F("No AMUs found."));
		led_color(5, 0, 0);
	}
}

void amu_dev_check_switch(void) {
	dev_sw0.update();
	dev_sw1.update();

	if (dev_sw0.changed() ) {
		if (dev_sw0.read() == LOW) {
			amu_dev_scan_for_devices();
		}
	}

	if (dev_sw1.changed() ) {
		if(dev_sw1.read() == LOW) {
			amu_dut_reset();
		}
	}
}

void amu_dev_read_usb(void) {
	if (Serial.available() > 0) {
		char c = Serial.read();

		// Handle backspace/delete
		if (c == BACKSPACE_CHAR || c == DELETE_CHAR) {
			if (line_pos == 0) {return;}
			line_pos--;
			Serial.write(BACKSPACE_SEQ);
			Serial.flush();
		// Handle newline/carriage return
		} else if (c == NEWLINE_CHAR || c == CARRIAGE_RETURN_CHAR) {
			Serial.write(NEWLINE_SEQ);
			Serial.flush();

			// Send complete line to SCPI
			if (line_pos > 0) {
				line_buffer[line_pos++] = NEWLINE_CHAR; // Add SCPI terminator
				amu_scpi_update_buffer((const char *) line_buffer, line_pos);
				line_pos = 0;  // Reset buffer
			}
		// Regular character 
		} else if (line_pos < LINE_BUFFER_SIZE - 1) {
			line_buffer[line_pos++] = c;
			Serial.write(c); // Echo character
			Serial.flush();
		}
	}
}

// Read load current
// Load current is measured using an INA199A3 200V/V current sense amplifier across a 10mΩ shunt resistor
// Current = Vadc / 200 / .01 = Vadc / 2
void amu_dev_read_load_current(void) {
	float load_current = analogRead(AMU_DEV_LOAD_CURRENT_PIN) / 4095. / 2.00;
}

int arduino_wire_transfer(TwoWire* wire, uint8_t address, uint8_t reg, uint8_t* data, size_t len, uint8_t read) {
    int rv = 4; // Other

    if (read) {
        if (len > 0) {
            wire->beginTransmission(address);
            wire->write(reg);
            rv = wire->endTransmission();
            if (rv != 0) {return rv;}
            if (wire->requestFrom(address, len) != len) {return 5;} // Timeout
            wire->readBytes(data, len);
            rv = 0;
        } else {
            wire->beginTransmission(address);
            rv = wire->endTransmission();
        }
    } else {
        wire->beginTransmission(address);
        wire->write(reg);
        if(len > 0) {wire->write(data, len);}
        rv = wire->endTransmission();
    }

    return rv;
}

int8_t twi_transfer(uint8_t address, uint8_t reg, uint8_t* data, size_t len, uint8_t read) {
	return arduino_wire_transfer(&AMU_DEV_TWI_BUS, address, reg, data, len, read);
}

void hal_usb_flush() {
	Serial.flush();
}

size_t hal_usb_write(const char* data, size_t len) {
	return Serial.write(data, len);
}

void led_color(uint8_t red, uint8_t green, uint8_t blue) {
	dev_board.setPixelColor(red, green, blue);
}

uint8_t amu_process_commands(uint16_t cmd) {
	uint16_t root = AMU_GET_CMD_ROOT(cmd); // Keep CMD_OFFSET bit and clear the read bit and lower nibble
	uint8_t branch = AMU_GET_CMD_BRANCH(cmd);

	// Place the cmd in the command register until the command is processed
	// Maintains consistency with the amu_command_complete() function for checking busy status from other threads
	amu_dev->amu_regs->command = (uint8_t) cmd;

	switch(root) {
		case CMD_SYSTEM: switch((CMD_SYSTEM_t) (root | branch)) {
			case CMD_SYSTEM_NO_CMD:             break;
			case CMD_SYSTEM_TWI_NUM_DEVICES:    if (cmd & CMD_READ) transfer_write_uint8_t(amu_get_num_devices());             break;
			case CMD_SYSTEM_FIRMWARE:           if (cmd & CMD_READ) transfer_write_str(amu_dev_getFirmwareStr(), AMU_FIRMWARE_STR_LEN);   break;
			case CMD_SYSTEM_SERIAL_NUM:         if (cmd & CMD_READ) transfer_write_str(amu_dev_getSerialNumStr(), AMU_SERIALNUM_STR_LEN); break;
			default: memset((void *) amu_dev_get_transfer_reg_ptr(), 0, AMU_TRANSFER_REG_SIZE); break;
		}

		default: memset((void *) amu_dev_get_transfer_reg_ptr(), 0, AMU_TRANSFER_REG_SIZE);
	}

	amu_command_complete();

	return 0;
}