#include <Wire.h>
#include <amulib.h>

// 0x00-0x07 are reserved by the I2C spec
#define AMU_DEV_TWI_SCAN_START_ADDRESS 0x08
#define AMU_DEV_TWI_SCAN_END_ADDRESS   0x20

#if defined(ESP32)
    // Replace with your board's SDA and SCL pins
    #define AMU_DEV_TWI_SDA_PIN 35
    #define AMU_DEV_TWI_SCL_PIN 37
#endif

void triggerVOC(Stream* s);
void printInfo(Stream* s);

AMU amu;

char notes[32];

bool amu_found = false;

void setup() {
    Serial.begin(AMU_BAUD_RATE);

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
    Wire.setTimeout(1000);

    while (!Serial);

    // The AMU constructor already registered the Wire transfer callback, so the
    // library-side scan can drive the bus as soon as Wire.begin() has run.
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