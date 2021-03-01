#include <amu.h>
#include <Wire.h>

#define AMU_TWI_BUS         Wire
#define AMU_DEVICES_MAX     8

void measureADCtemp(Stream* s);
void printInfo(AMU *dev, Stream* s);
int amu_wire_transfer(TwoWire* wire, uint8_t address, uint8_t reg, uint8_t* data, size_t len, uint8_t read);

int8_t twi_transfer(uint8_t address, uint8_t reg, uint8_t* data, size_t len, uint8_t read) { return (int8_t)amu_wire_transfer(&AMU_TWI_BUS, address, reg, data, len, read); }

size_t			hal_usb_write(const char* data, size_t len);
void			hal_usb_flush(void);

AMU amu[AMU_DEVICES_MAX];

char notes[64];

void setup() {

    Serial.begin(115200);

    AMU_TWI_BUS.begin();
    AMU_TWI_BUS.setClock(400000);
    AMU_TWI_BUS.setTimeout(250000);

    while (!Serial);

    AMU::amu_lib_init(twi_transfer);
    AMU::amu_scpi_init(hal_usb_write, hal_usb_flush);

    Serial.println("Scanning for devices...");

    amu_scan_for_devices(0x0B,0x20);

    if (amu_get_num_connected_devices() > 0) {
        for (uint8_t i = 0; i < amu_get_num_connected_devices(); i++) {

            amu[i].begin(amu_get_device_address(i + 1));

            amu[i].readNotes(notes, sizeof(notes));

            printInfo(&amu[i], &Serial);

            Serial.println();
        }
    }
    else {
        Serial.println(F("No AMUs found."));
    }

    
}

void loop() {

    while (Serial.available()) {
        amu_scpi_update(Serial.read());
    }

}

void measureADCtemp(Stream* s) {

    s->print(F("TEMP : "));    s->println(amu[0].measureBias(), 6);

}

int amu_wire_transfer(TwoWire* wire, uint8_t address, uint8_t reg, uint8_t* data, size_t len, uint8_t read) {

    uint8_t packetNum = 0;

    if (read) {
        if (len > 0) {

            wire->beginTransmission(address);
            wire->write(reg);
            wire->endTransmission();
            while (len > BUFFER_LENGTH) {
                wire->requestFrom(address, (uint8_t)BUFFER_LENGTH, (uint8_t)0);
                if (wire->available()) {
                    wire->readBytes(&data[packetNum * BUFFER_LENGTH], BUFFER_LENGTH);
                    packetNum++;
                    len -= BUFFER_LENGTH;
                }
            }
            wire->requestFrom(address, (uint8_t)len);
            if (wire->available())
                wire->readBytes(&data[packetNum * BUFFER_LENGTH], len);

        }
        else {
            wire->beginTransmission(address);
            return wire->endTransmission();
        }
    }
    else {
        wire->beginTransmission(address);
        wire->write(reg);
        while (len > BUFFER_LENGTH) {
            wire->write(&data[packetNum * BUFFER_LENGTH], BUFFER_LENGTH);
            len -= BUFFER_LENGTH;
        }
        wire->write(&data[packetNum * BUFFER_LENGTH], len);
        wire->endTransmission();
    }

    return 0;
}


void printInfo(AMU *dev, Stream * s) {

    ivsweep_config_t* sweep_config = dev->getSweepConfig();
    amu_dut_t* dut = dev->getDUT();

    char hardwareRevStr[4];
    switch (dev->getHardwareRevision()) {
    case AMU_HARDWARE_REVISION_ISC2:	    strcpy(hardwareRevStr, "0.1");		break;
    case AMU_HARDWARE_REVISION_AMU_1_0:     strcpy(hardwareRevStr, "1.0");		break;
    case AMU_HARDWARE_REVISION_AMU_1_1:     strcpy(hardwareRevStr, "1.1");		break;
    case AMU_HARDWARE_REVISION_AMU_2_0:     strcpy(hardwareRevStr, "2.0");		break;
    case AMU_HARDWARE_REVISION_AMU_2_1:     strcpy(hardwareRevStr, "2.1");		break;
    case AMU_HARDWARE_REVISION_AMU_SP_DEV:  strcpy(hardwareRevStr, "-SPDEV");	break;
    case AMU_HARDWARE_REVISION_AMU_SP:      strcpy(hardwareRevStr, "-SP");		break;
    default:							    strcpy(hardwareRevStr, "err");		break;
    }
    s->print(F("\nAMU "));	s->print(hardwareRevStr);	s->print(" found at address : 0x");	s->println(dev->getAddress(), HEX);
    s->print(F("\t:DUT:MANUFACTURER: "));		s->println(dut->manufacturer);
    s->print(F("\t:DUT:MODEL: "));				s->println(dut->model);
    s->print(F("\t:DUT:TECH: "));				s->println(dut->technology);
    s->print(F("\t:DUT:SERIAL: "));			    s->println(dut->serial);
    s->print(F("\t:DUT:NOTES "));				s->println(notes);
    s->print(F("\t:FIRMWARE: "));				s->println(dev->getFirmware());
    s->print(F("\t:SERIAL: "));				    s->println(dev->getSerialNumber());
    s->print(F("\t:SWEEP CONFIG: \t"));
    s->print(sweep_config->type);			    s->print("\t");
    s->print(sweep_config->numPoints);		    s->print("\t");
    s->print(sweep_config->delay);			    s->print("\t");
    s->print(sweep_config->ratio);			    s->print("\t");
    s->print(sweep_config->power);			    s->print("\t");
    s->print(sweep_config->dac_gain);		    s->print("\t");
    s->println(sweep_config->averages);
}

void hal_usb_flush() {
    Serial.flush();
}

size_t hal_usb_write(const char* data, size_t len) {
    return Serial.write(data, len);
}