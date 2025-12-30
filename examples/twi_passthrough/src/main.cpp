#include <Arduino.h>
#include <Wire.h>
#include <amulib.h>
#include <TaskScheduler.h>
#include <UMS3.h>
#include <Bounce2.h>

#define AMU_DEV_TWI_BUS         Wire
#define AMU_DEV_TWI_SDA_PIN     35
#define AMU_DEV_TWI_SCL_PIN     37
#define AMU_DEVICES_MAX         8

// Devboard Pins
#define AMU_DEV_AMU_RESET_PIN       6
#define AMU_DEV_LOAD_CURRENT_PIN    7

#define AMU_DEV_SW0_PIN             8
#define AMU_DEV_SW1_PIN             9


UMS3 dev_board;
// Scheduler ts;

void led_color(uint8_t red, uint8_t green, uint8_t blue);

void amu_dev_check_switch(void);
void amu_dev_read_load_current(void);
void amu_dev_read_usb(void);

void led_blink(void);


int arduino_wire_transfer(TwoWire* wire, uint8_t address, uint8_t reg, uint8_t* data, size_t len, uint8_t read);
int8_t twi_transfer(uint8_t address, uint8_t reg, uint8_t* data, size_t len, uint8_t read) { return (int8_t)arduino_wire_transfer(&AMU_DEV_TWI_BUS, address, reg, data, len, read); }

size_t hal_usb_write(const char* data, size_t len);
void hal_usb_flush(void);

uint8_t amu_process_comands(uint16_t cmd);


Bounce dev_sw0 = Bounce();
Bounce dev_sw1 = Bounce();

// Task tReadSwitch(5, TASK_FOREVER, &amu_dev_check_switch, &ts, true);
// // Task tReadCurrent(1000, TASK_FOREVER, &amu_dev_read_load_current, &ts, true);
// Task tReadUSB(5, TASK_FOREVER, &amu_dev_read_usb, &ts, true);
// Task tBlink(250, TASK_FOREVER, &led_blink, &ts, true);

void amu_dut_reset(void) {
  pinMode(AMU_DEV_AMU_RESET_PIN, OUTPUT);
    // Reset AMU device
  digitalWrite(AMU_DEV_AMU_RESET_PIN, HIGH);
  delay(100);
  digitalWrite(AMU_DEV_AMU_RESET_PIN, LOW);
  delay(250);
}

amu_device_t * amu_dev = nullptr;

void amu_dev_scan_for_devices(void) {

//   Serial.printf("Scanning for AMUs...\n");
  led_color(0, 0, 0);
  
  if (amu_scan_for_devices(0x10, 0x20) <= 1) {
    //   Serial.println(F("No AMUs found."));
      led_color(5, 0, 0);
  }
  else {
    int num_dut = amu_get_num_connected_devices();
    // Serial.printf("Found %u AMU(s)\n", num_dut);
    led_color(0, 0, 5);

    // for(uint8_t i = 0; i < num_dut; i++) {
    //   Serial.printf("Found AMU at address: 0x%02X\n", amu_get_device_address(i+1));  // Device 0 is this device
    // }
  }
}


void setup() {
  
    Serial.begin(115200);
    Serial.setTimeout(5000);

    dev_board.begin();

    dev_board.setPixelBrightness(255);

    // Enable the power to the RGB LED.
    // Off by default so it doesn't use current when the LED is not required.
    dev_board.setPixelPower(true);

    AMU_DEV_TWI_BUS.begin(AMU_DEV_TWI_SDA_PIN, AMU_DEV_TWI_SCL_PIN, 100000UL);  // Reduced from 400kHz to 100kHz
    AMU_DEV_TWI_BUS.setBufferSize(1024);
    AMU_DEV_TWI_BUS.setTimeOut(1000);  // Set timeout to 1 second

    dev_sw0.attach(AMU_DEV_SW0_PIN, INPUT_PULLUP);
    dev_sw1.attach(AMU_DEV_SW1_PIN, INPUT_PULLUP);

    dev_sw0.interval(20);
    dev_sw1.interval(20);

    analogRead(AMU_DEV_LOAD_CURRENT_PIN) / 4095.  / 2.00;;  // Dummy read to stabilize ADC

    while (!Serial);

    amu_dev = (amu_device_t *)amu_dev_init(twi_transfer);
    amu_dev->millis = millis;
    amu_dev->delay = delay;

    amu_dev->scpi_dev.write_cmd = hal_usb_write;
    amu_dev->scpi_dev.flush_cmd = hal_usb_flush;

    amu_dev->process_cmd = &amu_process_comands;

	  amu_dev_setDeviceTypeStr("AMU-DEV");

    amu_dev->amu_regs->hardware_revision = AMU_HARDWARE_REVISION_AMU_ESP32_DEV;

    led_color(0, 0, 5);  // Blue to indicate ready

    amu_dev_scan_for_devices();
    
    while(Serial.available()) Serial.read();  // Clear any input buffer
    
    // Serial.println("Setup complete. Send any text to test serial input...");

}

void loop() {
//   ts.execute();

    amu_dev_read_usb();

}

void amu_dev_read_usb(void) {

    if(Serial.available() > 0) {
        char c = Serial.read();
        // Serial.print(c);
        amu_scpi_update_buffer((const char *)&c, 1);
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

  if( dev_sw1.changed() ) {
    if(dev_sw1.read() == LOW) {

    }
  }
}
// Read load current
// Load current is measured using an INA199A3 200V/V current sense amplifier across a 10mΩ shunt resistor
// Current = Vadc / 200 / .01 = Vadc / 2
void amu_dev_read_load_current(void) {

  // float load_current = analogRead(AMU_DEV_LOAD_CURRENT_PIN) * 0.0402930403;
  float load_current = analogRead(AMU_DEV_LOAD_CURRENT_PIN) / 4095.  / 2.00;
  
//   // Teleplot print
//   Serial.printf(">AMU-CURR (mA):%.5e\n", load_current);

}


// Arduino wire transfer called by twi_transfer
int arduino_wire_transfer(TwoWire* wire, uint8_t address, uint8_t reg, uint8_t* data, size_t len, uint8_t read) {

    // if(read == 0) {
    //   Serial.printf("Writing to address 0x%02X, reg 0x%02X ", address, reg);
    //   for(size_t i = 0; i < len; i++) {
    //     Serial.printf("0x%02X ", data[i]);
    //   }
    //   Serial.println();
    // }
    

    if (read) {
        if (len > 0) {

            // Serial.printf("Reading from address 0x%02X, reg 0x%02X, len %u - ", address, reg, len);
            wire->beginTransmission(address);
            wire->write(reg);
            wire->endTransmission();
            wire->requestFrom(address, len);
            wire->readBytes(data, len);
            // for(size_t i=0; i<len; i++) {
            //   Serial.printf("0x%02X ", data[i]);
            // }
            // Serial.println();

        }
        else {
            wire->beginTransmission(address);
            return wire->endTransmission();
        }
    }
    else {
        // Serial.printf("Writing to address 0x%02X, reg 0x%02X, len %u data[0]: 0x%02X\n", address, reg, len, data[0]);
        wire->beginTransmission(address);
        wire->write(reg);
        if(len > 0) {
          wire->write(data, len);
        }
        wire->endTransmission();
    }

    return 0;
}


void hal_usb_flush() {
    Serial.flush();
}

size_t hal_usb_write(const char* data, size_t len) {
    return Serial.write(data, len);
}

void led_color(uint8_t red, uint8_t green, uint8_t blue) {
  dev_board.setPixelColor(green, red, blue);  // bug in USM3 swapped red/green
}

void led_blink(void) {

  static bool blink_on = false;

  if (blink_on == true) {
    led_color(0, 5, 0);
    blink_on = false;
  }
  else {  
    led_color(0, 0, 0);
    blink_on = true;
  }
}

uint8_t amu_process_comands(uint16_t cmd) {
	
	uint16_t root = AMU_GET_CMD_ROOT(cmd);		//keep CMD_OFFSET bit and clear the read bit and lower nibble
	uint8_t branch = AMU_GET_CMD_BRANCH(cmd);

	// Place the cmd in the command register until the command is processed
	// Maintains consistency with the amu_command_complete() function for checking busy status from other threads
	amu_dev->amu_regs->command = (uint8_t)cmd;
	
	switch(root) {

        case CMD_SYSTEM: switch((CMD_SYSTEM_t)(root | branch)) {
			case CMD_SYSTEM_NO_CMD:																																								break;
			// case CMD_SYSTEM_RESET:					hal_reset_soft();																															break;
			// case CMD_SYSTEM_XMEGA_FUSES:			if(cmd & CMD_READ) transfer_write_uint32_t(hal_xmega_read_fuses());																			break;
			// case CMD_SYSTEM_TWI_ADDRESS:			(cmd & CMD_READ) ? transfer_write_uint8_t(hal_twi_slave_get_address()) : hal_twi_slave_set_address(transfer_read_uint8_t());				break;
			case CMD_SYSTEM_TWI_NUM_DEVICES:		if(cmd & CMD_READ) transfer_write_uint8_t(amu_get_num_devices());																			break;
			// case CMD_SYSTEM_TWI_STATUS:				transfer_write_uint8_t(hal_twi_get_status());																					    	    break;
			case CMD_SYSTEM_FIRMWARE:				if(cmd & CMD_READ) transfer_write_str(amu_dev_getFirmwareStr(), AMU_FIRMWARE_STR_LEN);														break;		
			case CMD_SYSTEM_SERIAL_NUM:				if(cmd & CMD_READ) transfer_write_str(amu_dev_getSerialNumStr(), AMU_SERIALNUM_STR_LEN);													break;
			// case CMD_SYSTEM_TEMPERATURE:			transfer_write_float(hal_lradc_get_temperature());																							break;
			// case CMD_SYSTEM_TIME:					(cmd & CMD_READ) ? transfer_write_uint32_t(hal_millis()) : hal_millis_set(transfer_read_uint32_t());										break;
			// case CMD_SYSTEM_UTC_TIME:				(cmd & CMD_READ) ? transfer_write_uint32_t(hal_utc_get_time()) : hal_utc_set_time( transfer_read_uint32_t());								break;
			// case CMD_SYSTEM_LED_COLOR:				amu_leds_set_colors(transfer_read_amu_pid_t());																								break;
			// case CMD_SYSTEM_XMEGA_USER_SIGNATURES:	if(cmd & CMD_READ) hal_xmega_read_signature_json();																							break;
			// case CMD_SYSTEM_SLEEP:					hal_sleep();																															break;
			default: break;
		} break;


        default: memset((void *)amu_dev_get_transfer_reg_ptr(), 0x00, AMU_TRANSFER_REG_SIZE);
	}
	
	
	amu_command_complete();

	
	return 0;
}
