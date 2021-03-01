#include <Wire.h>
#include <amu.h>

#define AMU_TWI_BUS   Wire

void triggerVOC(Stream* s);
void printInfo(Stream* s);
int amu_wire_transfer(TwoWire* wire, uint8_t address, uint8_t reg, uint8_t* data, size_t len, uint8_t read);

int8_t twi_transfer(uint8_t address, uint8_t reg, uint8_t* data, size_t len, uint8_t read) { return (int8_t)amu_wire_transfer(&AMU_TWI_BUS, address, reg, data, len, read); }

AMU amu;

char notes[64];

bool amu_found = false;

void setup() {

    Serial.begin(115200);

    AMU_TWI_BUS.begin();
    AMU_TWI_BUS.setClock(400000);
    AMU_TWI_BUS.setTimeout(250000);

    while (!Serial);

    uint8_t address = 0x0B;

    while (address < 128) {
        if (twi_transfer(address, 0, NULL, 0, 1) == 0) {

            Serial.println("AMU init...");

            amu.begin(address, twi_transfer);

            amu.readNotes(notes, sizeof(notes));

            amu.setLEDmode(AMU_LED_PATTERN_QUICK_RGB_FADE);

            printInfo(&Serial);

            Serial.println();

            amu_found = true;

            break;
        }
        else {
            address++;
        }
    }
    if (amu_found == false) {
        Serial.println(F("No AMUs found."));
    }

}

void loop() {

    if (amu_found) {

        triggerVOC(&Serial);
        
        delay(1000);

        amu.setLEDmode(AMU_LED_PATTERN_GREEN_FLASH);
        
    }

}

void triggerVOC(Stream* s) {

    amu_meas_t measurementData;

    amu.triggerVoc();

    amu.waitUntilReady(250);

    measurementData = amu.readMeasurement();

    s->print(F("VOC : "));     s->print(measurementData.measurement, 6);      s->print("\t");
    s->print(F("TEMP : "));    s->println(measurementData.temperature, 6);

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


void printInfo(Stream* s) {

    ivsweep_config_t* sweep_config = amu.getSweepConfig();
    amu_dut_t* dut = amu.getDUT();

    char hardwareRevStr[4];
    switch (amu.getHardwareRevision()) {
    case AMU_HARDWARE_REVISION_ISC2:	strcpy(hardwareRevStr, "0.1");		break;
    case AMU_HARDWARE_REVISION_AMU_1_0: strcpy(hardwareRevStr, "1.0");		break;
    case AMU_HARDWARE_REVISION_AMU_1_1: strcpy(hardwareRevStr, "1.1");		break;
    case AMU_HARDWARE_REVISION_AMU_2_0: strcpy(hardwareRevStr, "2.0");		break;
    case AMU_HARDWARE_REVISION_AMU_2_1: strcpy(hardwareRevStr, "2.1");		break;
    default:							strcpy(hardwareRevStr, "err");		break;
    }
    s->print(F("\nAMU "));	s->print(hardwareRevStr);	s->print(" found at address : 0x");	s->println(amu.getAddress(), HEX);
    s->print(F("\t:DUT:MANUFACTURER: "));		s->println(dut->manufacturer);
    s->print(F("\t:DUT:MODEL: "));				s->println(dut->model);
    s->print(F("\t:DUT:TECH: "));				s->println(dut->technology);
    s->print(F("\t:DUT:SERIAL: "));			    s->println(dut->serial);
    s->print(F("\t:DUT:NOTES "));				s->println(notes);
    s->print(F("\t:FIRMWARE: "));				s->println(amu.getFirmware());
    s->print(F("\t:SERIAL: "));				    s->println(amu.getSerialNumber());
    s->print(F("\t:SWEEP CONFIG: \t"));
    s->print(sweep_config->type);			    s->print("\t");
    s->print(sweep_config->numPoints);		    s->print("\t");
    s->print(sweep_config->delay);			    s->print("\t");
    s->print(sweep_config->ratio);			    s->print("\t");
    s->print(sweep_config->power);			    s->print("\t");
    s->print(sweep_config->dac_gain);		    s->print("\t");
    s->println(sweep_config->averages);
}