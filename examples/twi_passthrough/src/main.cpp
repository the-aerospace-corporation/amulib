#include <amulib.h>
#include <Wire.h>

#define AMU_TWI_BUS         Wire
#define AMU_DEVICES_MAX     8


int arduino_wire_transfer(TwoWire* wire, uint8_t address, uint8_t reg, uint8_t* data, size_t len, uint8_t read);
int8_t twi_transfer(uint8_t address, uint8_t reg, uint8_t* data, size_t len, uint8_t read) { return (int8_t)arduino_wire_transfer(&AMU_TWI_BUS, address, reg, data, len, read); }

size_t hal_usb_write(const char* data, size_t len);
void hal_usb_flush(void);

amu_device_t * amu_dev;

char notes[64];

void setup() {

    Serial.begin(115200);

    AMU_TWI_BUS.begin();
    AMU_TWI_BUS.setClock(400000);
    AMU_TWI_BUS.setTimeout(250000);

    delay(500);

    while (!Serial);

    amu_dev = AMU::amu_lib_init(twi_transfer);

    AMU::amu_scpi_init(hal_usb_write, hal_usb_flush);

    if (amu_scan_for_devices(0x0B, 0x20) <= 1) {
        Serial.println(F("No AMUs found."));
    }
    else {
        Serial.printf("%i AMU Found at address: 0x%02X\n", amu_get_num_connected_devices(), amu_get_device_address(1));
    }

}

void loop() {

    char buffer[256];
    uint8_t cmd_len = 0;

    while (Serial.available()) {

        if (Serial.readBytesUntil('\n', buffer, sizeof(buffer)) > 0) {

            cmd_len = strlen(buffer);

            if (buffer[cmd_len - 1] == '\r') {
                buffer[cmd_len - 1] = '\0';
                cmd_len--;
            }


            if ((buffer[0] == '*') || (strcmp(buffer, "SYST:ERR?") == 0)) {
                strcat(buffer, "\n");
            }
            else {

                if (buffer[cmd_len - 1] == '?')
                    strcat(buffer, " (@1)\n");
                else
                    strcat(buffer, ",(@1)\n");
            }

            for (uint8_t i = 0; i < strlen(buffer); i++)
                amu_scpi_update(buffer[i]);
        }
    }

}

// Arduino wire transfer called by twi_transfer
int arduino_wire_transfer(TwoWire* wire, uint8_t address, uint8_t reg, uint8_t* data, size_t len, uint8_t read) {

    uint8_t packetNum = 0;

    if (read) {
        if (len > 0) {

            wire->beginTransmission(address);
            wire->write(reg);
            wire->endTransmission();
            wire->requestFrom(address, (uint8_t)len);
            wire->readBytes(data, len);
        }
        else {
            wire->beginTransmission(address);
            return wire->endTransmission();
        }
    }
    else {
        wire->beginTransmission(address);
        wire->write(reg);
        wire->write(data, len);
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
