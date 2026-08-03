#include <Wire.h>
#include <amulib.h>

// 0x00-0x07 are reserved by the I2C spec
#define AMU_DEV_TWI_SCAN_START_ADDRESS 0x08
#define AMU_DEV_TWI_SCAN_END_ADDRESS   0x7F

#if defined(ESP32)
    // Replace with your board's SDA and SCL pins
    #define AMU_DEV_TWI_SDA_PIN 35
    #define AMU_DEV_TWI_SCL_PIN 37
#endif

void triggerSweep(Stream* s);
void readSweepData(Stream* s, uint8_t numPoints);

void triggerVOC(Stream* s);
void triggerISC(Stream* s);
void printInfo(Stream* s);

AMU amu;

char notes[32];

bool amu_found = false;

void setup() {
    Serial.begin(AMU_BAUD_RATE);
    while (!Serial);

    // Board-specific I2C bus configuration. The ESP32 branch below matches the
    // TinyS3 dev board; on other cores, add your core's pin/buffer/timeout
    // setup here (or nothing - begin() with the default pins also works).
#if defined(ESP32)
    Wire.setPins(AMU_DEV_TWI_SDA_PIN, AMU_DEV_TWI_SCL_PIN);
    Wire.setBufferSize(1024);  // Reads are single transactions, so cover the largest register
    Wire.setTimeOut(1000);
#endif
    Wire.begin();
    Wire.setClock(AMU_TWI_FREQ_HZ);
    Wire.setTimeout(1000);

    if (amu_scan_for_devices(AMU_DEV_TWI_SCAN_START_ADDRESS, AMU_DEV_TWI_SCAN_END_ADDRESS) > 0) {
        Serial.println("AMU init...");

        amu.begin(amu_get_device_address(0));

        amu.readNotes(notes, sizeof(notes));

        amu.setLEDmode(AMU_LED_PATTERN_QUICK_RGB_FADE);

        printInfo(&Serial);

        Serial.println();

        amu_found = true;
    }
    else {
        Serial.println(F("No AMUs found."));
    }

}

void loop() {
    static bool loopActive = false;

    if (amu_found) {
        if (Serial.available()) {
            uint8_t b = Serial.read();
            switch (b) {
            case 't':
                Serial.println();
                triggerSweep(&Serial);
                readSweepData(&Serial, amu.getSweepConfig()->numPoints);
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
                loopActive = true;
                break;
            case 'l':
                loopActive = false;
                break;
            case 'I':
                printInfo(&Serial);
                break;
            }
        }

        if (loopActive) {
            Serial.println();
            triggerSweep(&Serial);
            readSweepData(&Serial, amu.getSweepConfig()->numPoints);
        }
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
    ivsweep_meta_t* meta = amu.readMeta();

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
    s->print(sweep_config->adc_averages);

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
    s->print(F("\nAMU Time(ms)\tVoltage (V)\tCurrent (A)"));

}

void readSweepData(Stream* s, uint8_t numPoints) {
    uint32_t timestamp[numPoints];
    float voltage[numPoints], current[numPoints];

    amu.readSweepTimestamps(timestamp);
    amu.readSweepVoltages(voltage);
    amu.readSweepCurrents(current);

    for (int i = 0; i < numPoints; i++) {
        s->print("\n");
        s->print(timestamp[i]);     s->print("\t");
        s->print(voltage[i], 6);    s->print("\t");
        s->print(current[i], 6);
    }

    s->println();
}

void printInfo(Stream* s) {
    ivsweep_config_t* sweep_config = amu.getSweepConfig();
    amu_dut_t* dut = amu.getDUT();

    // Hardware revision is BCD-encoded: high nibble = major, low nibble = minor (e.g. 0x21 -> "2.1")
    uint8_t hardwareRev = amu.getHardwareRevision();
    char hardwareRevStr[6];
    snprintf(hardwareRevStr, sizeof(hardwareRevStr), "%u.%u", hardwareRev >> 4, hardwareRev & 0x0F);
    s->print(F("\nAMU "));	s->print(hardwareRevStr);	s->print(" found at address: 0x");	s->println(amu.getAddress(), HEX);
    s->print(F("\tDUT:MANUFACTURER: "));		s->println(dut->manufacturer);
    s->print(F("\tDUT:MODEL: "));				s->println(dut->model);
    s->print(F("\tDUT:TECH: "));				s->println(dut->technology);
    s->print(F("\tDUT:SERIAL: "));			    s->println(dut->serial);
    s->print(F("\tDUT:NOTES "));				s->println(notes);
    s->print(F("\tFIRMWARE: "));				s->println(amu.getFirmware());
    s->print(F("\tSERIAL: "));				    s->println(amu.getSerialNumber());
    s->print(F("\tSWEEP CONFIG: \t"));
    s->print(sweep_config->type);			    s->print("\t");
    s->print(sweep_config->numPoints);		    s->print("\t");
    s->print(sweep_config->delay);			    s->print("\t");
    s->print(sweep_config->ratio);			    s->print("\t");
    s->print(sweep_config->power);			    s->print("\t");
    s->print(sweep_config->dac_gain);		    s->print("\t");
    s->println(sweep_config->adc_averages);
}