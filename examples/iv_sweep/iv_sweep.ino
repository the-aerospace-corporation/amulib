#include <Wire.h>
#include <amulib.h>

#define AMU_TWI_BUS   Wire

void triggerSweep(Stream* s);
void readSweepData(Stream* s, uint8_t numPoints);
void readSweepDataLowMemory(Stream* s, uint8_t numPoints);

void triggerVOC(Stream* s);
void triggerISC(Stream* s);
void printInfo(Stream* s);

int amu_wire_transfer(TwoWire* wire, uint8_t address, uint8_t reg, uint8_t* data, size_t len, uint8_t read);

int8_t twi_transfer(uint8_t address, uint8_t reg, uint8_t* data, size_t len, uint8_t read) { return (int8_t)amu_wire_transfer(&AMU_TWI_BUS, address, reg, data, len, read); }

AMU amu;

char notes[32];

bool amu_found = false;

void setup() {
  
    Serial.begin(115200);

    AMU_TWI_BUS.begin();

    while(!Serial);

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

    static uint8_t loopActive = 0;

    if (amu_found) {

        if (Serial.available()) {
            uint8_t b = Serial.read();
            switch (b) {
            case 't':
                Serial.println();
                triggerSweep(&Serial);
                break;
            case 'v':
                Serial.println();
                triggerVOC(&Serial);
                break;
            case 'i':
                Serial.println();
                triggerISC(&Serial);
                break;
            case 'L':
                loopActive = 1;
                break;
            case 'l':
                loopActive = 0;
                break;
            case 'I':
                printInfo(&Serial);
                break;
            }
            
        }

        loopTest(loopActive);

    }

}

void loopTest(uint8_t active) {

    if (active) {
        triggerVOC(&Serial);
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

void triggerISC(Stream* s) {

    amu_meas_t measurementData;

    amu.triggerIsc();

    amu.waitUntilReady(250);

    measurementData = amu.readMeasurement();

    s->print(F("ISC : "));     s->print(measurementData.measurement, 6);      s->print("\t");
    s->print(F("TEMP : "));    s->println(measurementData.temperature, 6);
}

void triggerSweep(Stream* s) {

    amu.triggerSweep();

    amu.waitUntilReady(5000);

    amu_int_volt_t int_volt = amu.measureInternalVoltages();
    ivsweep_config_t* sweep_config = amu.readSweepConfig();
    ivsweep_meta_t * meta = amu.readMeta();

    s->print(F("\nAddress\t"));	                s->print(amu.getAddress());
    s->print(F("\nManufacturer\t"));		    s->print(amu.getDutManufacturer());
    s->print(F("\nModel\t"));				    s->print(amu.getDutModel());
    s->print(F("\nJunction\t"));			    s->print(amu.getDutJunction());
    s->print(F("\nSerial Number\t"));			s->print(amu.getDutSerialNumber());
    s->print(F("\nNotes "));				    s->print(notes);
    s->print(F("\nTimestamp: "));				s->print(millis());

    s->print(F("\nSweep Config\t"));
    s->print(sweep_config->type);			s->print("\t");
    s->print(sweep_config->numPoints);		s->print("\t");
    s->print(sweep_config->delay);			s->print("\t");
    s->print(sweep_config->ratio);			s->print("\t");
    s->print(sweep_config->power);			s->print("\t");
    s->print(sweep_config->dac_gain);		s->print("\t");
    s->print(sweep_config->averages);

    s->print(F("\nAMU Internal\t"));
    s->print(int_volt.avdd, 6);                s->print("\t");
    s->print(int_volt.iovdd, 6);               s->print("\t");
    s->print(int_volt.aldo, 6);                s->print("\t");
    s->print(int_volt.dldo, 6);

    s->print(F("\nVoc (V)\t"));                s->print(meta->voc, 6);
    s->print(F("\nIsc (A)\t"));                s->print(meta->isc);
    s->print(F("\nJsc (A/cm^2)\t"));           s->print(meta->isc / sweep_config->area, 6);
    s->print(F("\nVmax (V)\t"));               s->print(meta->vmax, 6);
    s->print(F("\nImax (A)\t"));               s->print(meta->imax, 6);
    s->print(F("\nPmax (W)\t"));               s->print(meta->pmax, 6);
    s->print(F("\nFF\t"));                     s->print(meta->ff, 6);
    s->print(F("\nEFF (%)\t"));                s->print(meta->eff, 6);

    s->print(F("\nCell Area (cm^2)\t"));       s->print(sweep_config->area, 6);
    s->print(F("\nCell Temp (C)\t"));          s->print(meta->tsensor_start, 6);  s->print("\t");     s->print(meta->tsensor_end, 6);
    s->print(F("\nAM0 constant (W/cm^2)\t"));  s->print(sweep_config->am0, 6);
    s->print(F("\nADC\t"));                    s->print(meta->adc, 6);
    s->print(F("\nCRC-32\t0x"));               s->print(meta->crc, HEX);
    s->print(F("\nAMU Time(ms)\tVoltage (V)\tCurrent (A)"));

}

void readSweepData(Stream* s, uint8_t numPoints) {

    uint32_t* timestamp = amu.readSweepTimestamps();
    float* voltage = amu.readSweepVoltages();
    float* current = amu.readSweepCurrents();

    for (int i = 0; i < numPoints; i++) {
        s->print("\n");
        s->print(timestamp[i]);     s->print("\t");
        s->print(voltage[i], 6);    s->print("\t");
        s->print(current[i], 6);
    }

    s->println();
}

void readSweepDataLowMemory(Stream* s, uint8_t numPoints) {

    ivsweep_datapoint_t datapoint;

    for (uint8_t i = 0; i < numPoints / 10; i++) {

        amu.loadSweepDatapoints(i * 10);

        AMU_TWI_BUS.beginTransmission(amu.getAddress());
        AMU_TWI_BUS.write(AMU_REG_TRANSFER_PTR);
        AMU_TWI_BUS.endTransmission();

        for (uint8_t j = 0; j < 10; j++) {
            AMU_TWI_BUS.requestFrom(amu.getAddress(), sizeof(ivsweep_datapoint_t));
            AMU_TWI_BUS.readBytes((uint8_t*)&datapoint, sizeof(ivsweep_datapoint_t));
            s->print("\n");
            s->print(datapoint.voltage, 6);    s->print("\t");
            s->print(datapoint.current, 6);
        }
    }

    s->println();

}


int amu_wire_transfer(TwoWire *wire, uint8_t address, uint8_t reg, uint8_t *data, size_t len, uint8_t read) {
    
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


void printInfo(Stream * s) {
        
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