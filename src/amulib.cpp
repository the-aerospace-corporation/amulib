#include "amulib.h"

#ifdef __AMU_CLASS__

#include <math.h>
#include <string.h>

AMU::errorPrintFncPtr_t AMU::errorPrintFncPtr = nullptr;
AMU::resetFncPtr_t AMU::amuResetFncPtr = nullptr;
AMU::resetFncPtr_t AMU::eyasResetFncPtr = nullptr;

#define AMU_ERROR(...) do { if (AMU::errorPrintFncPtr) { AMU::errorPrintFncPtr(__VA_ARGS__); } } while (0)

#ifdef ARDUINO
AMU::AMU(TwoWire* wire) {
	initState();
	amu_dev = devInit(wire);
}
#else
AMU::AMU(void) {
	initState();
}
#endif // ARDUINO

#ifdef __AMU_REMOTE_DEVICE__
AMU::AMU(amu_transfer_fptr_t i2c_transfer_func) {
	initState();
	amu_dev = amu_lib_init(i2c_transfer_func);
}
#endif // __AMU_REMOTE_DEVICE__

void AMU::initState(void) {
	address = AMU_NO_ADDRESS_MATCH;
	amu_dev = nullptr;
	hardware_revision = (amu_hardware_revision_t) 0;

	memset(serial_number, 0, sizeof(serial_number));
	memset(firmware, 0, sizeof(firmware));
	memset(&dut, 0, sizeof(dut));
	memset(&sweep_config, 0, sizeof(sweep_config));
	memset(&meta, 0, sizeof(meta));
	memset(&sun_sensor, 0, sizeof(sun_sensor));
}

amu_device_t* AMU::amu_lib_init(amu_transfer_fptr_t i2c_transfer_func) {
#ifdef ARDUINO
	return devInit(i2c_transfer_func);
#else
	return (amu_device_t*) amu_dev_init(i2c_transfer_func);
#endif
}

#ifdef __AMU_USE_SCPI__
amu_scpi_dev_t* AMU::amu_scpi_init(size_t(*write_cmd)(const char* data, size_t len), void(*flush_cmd)(void)) {
	return amu_scpi_dev_init(write_cmd, flush_cmd);
}
#endif // __AMU_USE_SCPI__

#ifdef ARDUINO

static TwoWire *amu_wire = &Wire;

static int8_t wireStatus(uint8_t endTransmissionResult) {
	switch (endTransmissionResult) {
		case 0:		return AMU_TWI_STATUS_OK;
		case 1:		return AMU_TWI_STATUS_DATA_TOO_LONG;
		case 2:		return AMU_TWI_STATUS_NACK_ADDR;
		case 3:		return AMU_TWI_STATUS_NACK_DATA;
		case 5:		return AMU_TWI_STATUS_TIMEOUT;
		default:	return AMU_TWI_STATUS_ERROR;
	}
}

int8_t AMU::wireTransfer(TwoWire *wire, uint8_t address, uint8_t reg, uint8_t* data, size_t len, uint8_t read) {
	int8_t rv = AMU_TWI_STATUS_ERROR;

	if (read) {
		if (len > 0) {
			wire->beginTransmission(address);
			wire->write(reg);
			rv = wireStatus(wire->endTransmission());
			if (rv != AMU_TWI_STATUS_OK) {return rv;}

			if (wire->requestFrom(address, len) != len) {return AMU_TWI_STATUS_TIMEOUT;}
			if (wire->readBytes(data, len) != len) {return AMU_TWI_STATUS_TIMEOUT;} // Short read

			rv = AMU_TWI_STATUS_OK;
		// Device scan
		} else {
			wire->beginTransmission(address);
			rv = wireStatus(wire->endTransmission());
		}
	} else {
		wire->beginTransmission(address);
		wire->write(reg);
		if (len > 0 && wire->write(data, len) != len) {return AMU_TWI_STATUS_DATA_TOO_LONG;}
		rv = wireStatus(wire->endTransmission());
	}

	return rv;
}

int8_t AMU::twiTransfer(uint8_t address, uint8_t reg, uint8_t* data, size_t len, uint8_t read) {
	return wireTransfer(amu_wire, address, reg, data, len, read);
}

amu_device_t *AMU::devInit(TwoWire *wire) {
	amu_wire = wire;
	return devInit(twiTransfer);
}

amu_device_t* AMU::devInit(amu_transfer_fptr_t i2c_transfer_func) {
	amu_device_t* dev = (amu_device_t*) amu_dev_init(i2c_transfer_func);
	dev->delay = delay;
	dev->millis = millis;
	return dev;
}

static Stream *amu_usb = &Serial;

void AMU::usbInit(Stream& stream) {
	amu_usb = &stream;
}

size_t AMU::usbWrite(const char* data, size_t len) {
	return amu_usb->write(data, len);
}

void AMU::usbFlush(void) {
	amu_usb->flush();
}

#ifdef __AMU_USE_SCPI__
void AMU::scpiProcessStream(Stream& stream) {
	usbInit(stream);
	scpiProcessStream();
}

void AMU::scpiProcessStream(void) {
	static char line_buffer[AMULIBC_SCPI_INPUT_BUFFER_LENGTH];
	static size_t line_pos = 0;

	if (amu_usb->available() <= 0) {return;}

	char c = amu_usb->read();

	// Handle backspace/delete
	if (c == '\b' || c == 0x7F) {
		if (line_pos == 0) {return;}
		line_pos--;
		amu_usb->write("\b \b");
		amu_usb->flush();
	// Handle newline/carriage return
	} else if (c == '\n' || c == '\r') {
		amu_usb->write("\r\n");
		amu_usb->flush();

		// Send complete line to SCPI
		if (line_pos > 0) {
			line_buffer[line_pos++] = '\n'; // Add SCPI terminator
			amu_scpi_update_buffer((const char*) line_buffer, line_pos);
			line_pos = 0; // Reset buffer
		}
	// Regular character
	} else if (line_pos < AMULIBC_SCPI_INPUT_BUFFER_LENGTH - 1) {
		line_buffer[line_pos++] = c;
		amu_usb->write(c); // Echo character
		amu_usb->flush();
	}
}
#endif // __AMU_USE_SCPI__

#endif // ARDUINO

#ifdef __AMU_REMOTE_DEVICE__

void AMU::begin(uint8_t twiAddress) {
	address = twiAddress;

	readFirmwareStr();

	readSerialStr();

	hardware_revision = (amu_hardware_revision_t) read_twi_reg<uint8_t>(AMU_REG_SYSTEM_HARDWARE_REVISION);

	dut = read_twi_reg<amu_dut_t>(AMU_REG_DUT);

	readSweepConfig();
}

void AMU::begin(uint8_t twiAddress, amu_transfer_fptr_t i2c_transfer_func) {
	amu_dev = amu_lib_init(i2c_transfer_func);

	begin(twiAddress);
}

int8_t AMU::waitUntilReady(uint32_t timeout) {
	return amu_dev_wait_until_ready(address, timeout, AMU_DEV_WAIT_INITIAL_DELAY_MS, AMU_DEV_WAIT_MAX_DELAY_MS);
}

int8_t AMU::busy() {
	return amu_dev_busy(address);
}

int8_t AMU::sleep(void) {
	return sendCommand((CMD_t) CMD_SYSTEM_SLEEP);
}

int8_t AMU::reset(void) {
	return sendCommand((CMD_t) CMD_SYSTEM_RESET);
}

char* AMU::readNotes(char* notes, uint8_t len) {
	return query<char>((CMD_t) CMD_DUT_NOTES, notes, len);
}

char* AMU::readNotes(char* notes) {
	return query<char>((CMD_t) CMD_DUT_NOTES, notes, AMU_NOTES_SIZE);
}

uint8_t AMU::getDutTsensorType(void) {
	return query<uint8_t>((CMD_t) CMD_DUT_TSENSOR_TYPE);
}

uint8_t AMU::getDutTsensorNumber(void) {
	return query<uint8_t>((CMD_t) CMD_DUT_TSENSOR_NUMBER);
}

amu_coeff_t AMU::getDutTsensorFit(void) {
	return query<amu_coeff_t>((CMD_t) CMD_DUT_TSENSOR_FIT);
}

char* AMU::readSerialStr() {
	return query<char>((CMD_t) CMD_SYSTEM_SERIAL_NUM, (char*) &serial_number, (size_t) AMU_SERIALNUM_STR_LEN);
}

char* AMU::readFirmwareStr() {
	return query<char>((CMD_t) CMD_SYSTEM_FIRMWARE, (char*) &firmware, (size_t) AMU_FIRMWARE_STR_LEN);
}

// ---------------------------------------------------------------------------
// Device configuration
// ---------------------------------------------------------------------------

int8_t AMU::setActiveChannels(uint16_t channels) {
	return write_twi_reg<uint16_t>(AMU_REG_SYSTEM_ADC_ACTIVE_CHANNELS, channels);
}

int8_t AMU::setTimeStamp(uint32_t timestamp) {
	return sendCommand((CMD_t) CMD_SYSTEM_UTC_TIME, &timestamp, sizeof(uint32_t));
}

int8_t AMU::setLEDcolor(float red, float grn, float blu) {
	amu_pid_t colors = {red, grn, blu};
	return sendCommand((CMD_t) CMD_SYSTEM_LED_COLOR, (void *) &colors, sizeof(amu_pid_t));
}

int8_t AMU::setLEDmode(amu_led_pattern_t mode) {
	return sendCommand((CMD_t) ((uint16_t) CMD_SYSTEM_LED + (uint16_t) mode));
}

int8_t AMU::setDACState(bool state) {
	uint8_t value = state ? 1 : 0;
	return sendCommand((CMD_t) CMD_AUX_DAC_STATE, &value, sizeof(value));
}

bool AMU::getDACState(void) {
	return query<uint8_t>((CMD_t) CMD_AUX_DAC_STATE) != 0;
}

int8_t AMU::setDACVoltage(float voltage) {
	return sendCommand((CMD_t) CMD_AUX_DAC_VOLTAGE, &voltage, sizeof(voltage));
}

float AMU::getDACVoltage(void) {
	return query<float>((CMD_t) CMD_AUX_DAC_VOLTAGE);
}

int8_t AMU::setHeaterState(bool state) {
	uint8_t value = state ? 1 : 0;
	return sendCommand((CMD_t) CMD_AUX_HEATER_STATE, &value, sizeof(value));
}

bool AMU::getHeaterState(void) {
	return query<uint8_t>((CMD_t) CMD_AUX_HEATER_STATE) != 0;
}

int8_t AMU::enableBluetooth(bool state) {
	uint8_t value = state ? 1 : 0;
	return sendExtCommand(CMD_EXT_BLE_STATE, &value, sizeof(value));
}

bool AMU::bluetoothEnabled(void) {
	uint8_t state = 0;

	if (queryExtCommand(CMD_EXT_BLE_STATE, NULL, 0, &state, sizeof(state)) != 0) {
		return false;
	}

	return (state != 0);
}

int8_t AMU::setBluetoothPasskey(uint32_t passkey) {
	return sendExtCommand(CMD_EXT_BLE_PASSKEY, &passkey, sizeof(passkey));
}

uint32_t AMU::bluetoothPasskey(void) {
	uint32_t passkey = 0;

	if (queryExtCommand(CMD_EXT_BLE_PASSKEY, NULL, 0, &passkey, sizeof(passkey)) != 0) {
		return 0;
	}

	return passkey;
}

uint32_t AMU::updateBegin(uint32_t size) {
	uint32_t device_chunk = 0;

	if (queryExtCommand(CMD_EXT_FIRMWARE_BEGIN, &size, sizeof(size),
	                    &device_chunk, sizeof(device_chunk)) != 0) {
		return 0;
	}

	return device_chunk;
}

uint32_t AMU::updateData(const void* data, uint16_t len) {
	if ((len == 0) || (len > AMU_OTA_CHUNK_TWI)) {
		AMU_ERROR("Firmware chunk %u exceeds %u\n", (unsigned) len, (unsigned) AMU_OTA_CHUNK_TWI);
		return 0;
	}

	uint8_t payload[sizeof(len) + AMU_OTA_CHUNK_TWI];
	memcpy(payload, &len, sizeof(len));
	memcpy(&payload[sizeof(len)], data, len);

	uint32_t written = 0;
	if (queryExtCommand(CMD_EXT_FIRMWARE_DATA, payload, sizeof(len) + len,
	                    &written, sizeof(written)) != 0) {
		return 0;
	}

	return written;
}

int32_t AMU::updateEnd(void) {
	int32_t rv = -1;

	if (queryExtCommand(CMD_EXT_FIRMWARE_END, NULL, 0, &rv, sizeof(rv)) != 0) {
		return -1;
	}

	return rv;
}

bool AMU::updateState(uint8_t *state, uint32_t *written, uint32_t *total, uint32_t *chunk) {
	amu_fw_status_t status;

	if (queryExtCommand(CMD_EXT_FIRMWARE_STATE, NULL, 0, &status, sizeof(status)) != 0) {return false;}

	if (state != NULL)   {*state = status.state;}
	if (written != NULL) {*written = status.written;}
	if (total != NULL)   {*total = status.total;}
	if (chunk != NULL)   {*chunk = status.chunk;}

	return true;
}

int8_t AMU::updateAbort(void) {
	return sendExtCommand(CMD_EXT_FIRMWARE_ABORT);
}

int8_t AMU::updateCommit(void) {
	return sendExtCommand(CMD_EXT_FIRMWARE_COMMIT);
}

bool AMU::updateCommitted(void) {
	uint8_t committed = 0;

	if (queryExtCommand(CMD_EXT_FIRMWARE_COMMIT, NULL, 0, &committed, sizeof(committed)) != 0) {
		return false;
	}

	return (committed != 0);
}

// ---------------------------------------------------------------------------
// ADC measurements
// ---------------------------------------------------------------------------

float AMU::measureVoltage() { return query<float>((CMD_t) CMD_MEAS_CH_VOLTAGE);	}
float AMU::measureCurrent() { return query<float>((CMD_t) CMD_MEAS_CH_CURRENT); }
float AMU::measureTSensor() { return query<float>((CMD_t) CMD_MEAS_CH_TSENSOR_0); }
float AMU::measureTSensor0() { return query<float>((CMD_t) CMD_MEAS_CH_TSENSOR_0); }
float AMU::measureTSensor1() { return query<float>((CMD_t) CMD_MEAS_CH_TSENSOR_1); }
float AMU::measureTSensor2() { return query<float>((CMD_t) CMD_MEAS_CH_TSENSOR_2); }
float AMU::measureSSTL() { return query<float>((CMD_t) CMD_MEAS_CH_SS_TL); }
float AMU::measureSSBL() { return query<float>((CMD_t) CMD_MEAS_CH_SS_BL); }
float AMU::measureSSBR() { return query<float>((CMD_t) CMD_MEAS_CH_SS_BR); }
float AMU::measureSSTR() { return query<float>((CMD_t) CMD_MEAS_CH_SS_TR); }
float AMU::measureBias() { return query<float>((CMD_t) CMD_MEAS_CH_BIAS); }
float AMU::measureOffset() { return query<float>((CMD_t) CMD_MEAS_CH_OFFSET); }

float AMU::measureTemperature() { return query<float>((CMD_t) CMD_MEAS_CH_TEMP); }
float AMU::measureAvdd() { return query<float>((CMD_t) CMD_MEAS_CH_AVDD); }
float AMU::measureIOvdd() { return query<float>((CMD_t) CMD_MEAS_CH_IOVDD); }
float AMU::measureAldo() { return query<float>((CMD_t) CMD_MEAS_CH_ALDO); }
float AMU::measureDldo() { return query<float>((CMD_t) CMD_MEAS_CH_DLDO); }

float AMU::measureChannel(uint8_t channel) {
	if (channel < AMU_ADC_CH_NUM) {
		return query<float>((CMD_t) (CMD_MEAS_CH_VOLTAGE + channel));
	}
	else
		return -1.000;
}

int8_t AMU::measureActiveChannels() {
	return sendCommand((CMD_t) CMD_EXEC_MEAS_ACTIVE_CHANNELS);
}

amu_int_volt_t AMU::measureInternalVoltages(void) {
	return query<amu_int_volt_t>((CMD_t) CMD_EXEC_MEAS_INTERNAL_VOLTAGES);
}

float AMU::measureSystemTemperature() {
	return query<float>((CMD_t) CMD_SYSTEM_TEMPERATURE);
}

uint8_t AMU::getPGA(AMU_ADC_CH_t channel) {
	if (channel < AMU_ADC_CH_NUM)
		return (1<<queryChannel<uint8_t>((CMD_t) CMD_ADC_CH_PGA, channel));
	else
		return 0;
}

uint32_t AMU::getADCstatus(void) {
	return read_twi_reg<uint32_t>((uint8_t) AMU_REG_SYSTEM_STATUS_HRADC);
}

// ---------------------------------------------------------------------------
// EYAS pressure sensor
// ---------------------------------------------------------------------------

press_data_t AMU::measurePressureSensor(void) {
	return query<press_data_t>((CMD_t) CMD_EXEC_MEAS_PRESSURE_SENSOR);
}

float AMU::measurePressure(void) {
	press_data_t press_data = measurePressureSensor();
	return press_data.pressure;
}

float AMU::measureHumidity(void) {
	press_data_t press_data = measurePressureSensor();
	return press_data.humidity;

}

float AMU::measurePSTemperature(void) {
	press_data_t press_data = measurePressureSensor();
	return press_data.temperature;
}

// ---------------------------------------------------------------------------
// Sun sensor
// ---------------------------------------------------------------------------

quad_photo_sensor_t AMU::measureSunSensor() {
	return query<quad_photo_sensor_t>((CMD_t) CMD_EXEC_MEAS_SUN_SENSOR, &sun_sensor);
}

ss_angle_t *AMU::readSunSensorAngles() {
	sun_sensor.angle.yaw = read_twi_reg<float>(AMU_REG_SUNSENSOR_YAW);
	sun_sensor.angle.pitch = read_twi_reg<float>(AMU_REG_SUNSENSOR_PITCH);
	return &sun_sensor.angle;
}

quad_photo_sensor_t *AMU::readSunSensorMeasurement() {
	measureSunSensor();
	return &sun_sensor;
}

float AMU::getPhotoDiodeVoltage(uint8_t n) {
	if (n < 4) {
		return sun_sensor.diode[n];
	}

	return -1.000;
}

bool AMU::goodSunAngle(float minAngle) {
	if (isnan(getYaw()) || isnan(getPitch())) {return false;}

	return ((getYawAbs() < minAngle) && (getPitchAbs() < minAngle));
}

float AMU::getDACgainCorrection(void) {
	return query<float>((CMD_t) CMD_AUX_DAC_GAIN_CORRECTION);
}

amu_coeff_t AMU::getYawCoefficients(void) {return query<amu_coeff_t>((CMD_t) CMD_AUX_SUNSENSOR_FIT_YAW_COEFF);}
amu_coeff_t AMU::getPitchCoefficients(void) {return query<amu_coeff_t>((CMD_t) CMD_AUX_SUNSENSOR_FIT_PITCH_COEFF);}

float AMU::getSSHVal(void) {return query<float>((CMD_t) CMD_AUX_SUNSENSOR_HVAL);}
float AMU::getSSRVal(void) {return query<float>((CMD_t) CMD_AUX_SUNSENSOR_RVAL);}

int8_t AMU::setSSThreshold(float threshold) {
	return sendCommand((CMD_t) CMD_AUX_SUNSENSOR_THRESHOLD, &threshold, sizeof(threshold));
}

float AMU::getSSThreshold(void) {
	return query<float>((CMD_t) CMD_AUX_SUNSENSOR_THRESHOLD);
}

int8_t AMU::triggerIsc(void) {
	return sendCommand((CMD_t) CMD_SWEEP_TRIG_ISC);
}

int8_t AMU::triggerVoc(void) {
	return sendCommand((CMD_t) CMD_SWEEP_TRIG_VOC);
}

int8_t AMU::triggerSweep(void) {
	return sendCommand((CMD_t) CMD_SWEEP_TRIG_SWEEP);
}

amu_meas_t AMU::measureIsc(void) {
	return query<amu_meas_t>((CMD_t) CMD_SWEEP_TRIG_ISC);
}

amu_meas_t AMU::measureVoc(void) {
	return query<amu_meas_t>((CMD_t) CMD_SWEEP_TRIG_VOC);
}

amu_meas_t AMU::readMeasurement(void) {return read_twi_reg<amu_meas_t>(AMU_REG_TRANSFER_PTR); }

ivsweep_config_t *AMU::readSweepConfig() {sweep_config = read_twi_reg<ivsweep_config_t>(AMU_REG_DATA_PTR_SWEEP_CONFIG); return &sweep_config;}
ivsweep_meta_t *AMU::readMeta() {meta = read_twi_reg<ivsweep_meta_t>(AMU_REG_DATA_PTR_SWEEP_META); return &meta;}

float AMU::readIsc() {meta.isc = read_twi_reg<float>(AMU_REG_SWEEP_META_ISC); return meta.isc;}
float AMU::readVoc() {meta.voc = read_twi_reg<float>(AMU_REG_SWEEP_META_VOC); return meta.voc;}

static inline uint8_t clampSweepPoints(uint8_t numPoints) {
	return (numPoints > IVSWEEP_MAX_POINTS) ? (uint8_t) IVSWEEP_MAX_POINTS : numPoints;
}

uint32_t *AMU::readSweepTimestamps(uint32_t* data) {return read_twi_reg<uint32_t>(AMU_REG_DATA_PTR_TIMESTAMP, data, sizeof(uint32_t) * clampSweepPoints(sweep_config.numPoints));}
float *AMU::readSweepVoltages(float* data) {return read_twi_reg<float>(AMU_REG_DATA_PTR_VOLTAGE, data, sizeof(float) * clampSweepPoints(sweep_config.numPoints));}
float *AMU::readSweepCurrents(float* data) {return read_twi_reg<float>(AMU_REG_DATA_PTR_CURRENT, data, sizeof(float) * clampSweepPoints(sweep_config.numPoints));}
float *AMU::readSweepYaws(float* data) {return read_twi_reg<float>(AMU_REG_DATA_PTR_SS_YAW, data, sizeof(float) * clampSweepPoints(sweep_config.numPoints));}
float *AMU::readSweepPitches(float* data) {return read_twi_reg<float>(AMU_REG_DATA_PTR_SS_PITCH, data, sizeof(float) * clampSweepPoints(sweep_config.numPoints));}

ivsweep_packet_t* AMU::readSweepIV(ivsweep_packet_t* sweep_packet) {
	readSweepVoltages(sweep_packet->voltage);
	readSweepCurrents(sweep_packet->current);
	return sweep_packet;
}

ivsweep_packet_t* AMU::readSweepSunAngle(ivsweep_packet_t* sweep_packet) {
#ifndef __AMU_LOW_MEMORY__
	readSweepYaws(sweep_packet->yaw);
	readSweepPitches(sweep_packet->pitch);
#else
	AMU_ERROR("Error: sweep sun angles are unavailable in low memory builds\n");
#endif
	return sweep_packet;
}

ivsweep_packet_t* AMU::readSweepAll(ivsweep_packet_t* sweep_packet) {
	readSweepIV(sweep_packet);
#ifndef __AMU_LOW_MEMORY__
	readSweepSunAngle(sweep_packet);
#endif
	return sweep_packet;
}

void AMU::loadSweepDatapoints(uint8_t offset) {
	sendCommand((CMD_t) CMD_SWEEP_DATAPOINT_LOAD, &offset, 1);
}

int8_t AMU::sendCommand(CMD_t cmd) {
	static uint8_t wait_error = 0;

	wait_error = waitUntilReady(1000);

	if (wait_error != 0) {
		AMU_ERROR("Wait until ready error: %u\n", wait_error);
	}
	return amu_dev_send_command(address, cmd);
}

int8_t AMU::sendCommand(CMD_t cmd, void *params, uint8_t param_len) {
	amu_dev_transfer(address, AMU_REG_TRANSFER_PTR, (uint8_t*) params, param_len, AMU_TWI_TRANSFER_WRITE);
	return sendCommand(cmd);
}

int8_t AMU::sendCommandandWait(CMD_t cmd, uint32_t wait) {
	sendCommand(cmd);
	return waitUntilReady(wait);
}

int8_t AMU::sendExtCommand(CMD_EXT_t ext) {
	return sendExtCommand(ext, NULL, 0);
}

int8_t AMU::sendExtCommand(CMD_EXT_t ext, const void* params, size_t param_len) {
	if ((param_len + AMU_EXT_PAYLOAD) > AMU_TRANSFER_REG_SIZE) {
		AMU_ERROR("Extended payload %u exceeds the transfer register\n", (unsigned) param_len);
		return -1;
	}

	// Staged as one write: the device reads the opcode and payload together
	amu_ext_cmd_t ext_value = (amu_ext_cmd_t) ext;
	memcpy((void *) amu_dev->transfer_reg, &ext_value, sizeof(ext_value));

	if ((params != NULL) && (param_len > 0)) {
		memcpy((void *) &amu_dev->transfer_reg[AMU_EXT_PAYLOAD], params, param_len);
	}

	int8_t rv = amu_dev_transfer(address, AMU_REG_TRANSFER_PTR, (uint8_t*) amu_dev->transfer_reg,
	                             AMU_EXT_PAYLOAD + param_len, AMU_TWI_TRANSFER_WRITE);
	if (rv != 0) {return rv;}

	return sendCommand((CMD_t) CMD_SYSTEM_EXTENDED);
}

int8_t AMU::queryExtCommand(CMD_EXT_t ext, const void* params, size_t param_len, void* response, size_t response_len) {
	if (!amu_dev || !amu_dev->transfer_reg) {
		AMU_ERROR("Error: amu_dev or transfer_reg is null\n");
		return -1;
	}

	if ((param_len + AMU_EXT_PAYLOAD) > AMU_TRANSFER_REG_SIZE) {
		AMU_ERROR("Extended payload %u exceeds the transfer register\n", (unsigned) param_len);
		return -1;
	}

	amu_ext_cmd_t ext_value = (amu_ext_cmd_t) ext;
	memcpy((void *) amu_dev->transfer_reg, &ext_value, sizeof(ext_value));

	if ((params != NULL) && (param_len > 0)) {
		memcpy((void *) &amu_dev->transfer_reg[AMU_EXT_PAYLOAD], params, param_len);
	}

	// The opcode and payload are the command's input, so they go in as commandDataLen
	int8_t rv = amu_dev_query_command(address, (CMD_t) CMD_SYSTEM_EXTENDED,
	                                  AMU_EXT_PAYLOAD + param_len, response_len);
	if (rv != 0) {
		AMU_ERROR("Extended command 0x%04X failed with error: %d\n", (unsigned) ext, rv);
		return rv;
	}

	if ((response != NULL) && (response_len > 0)) {
		memcpy(response, (const void *) amu_dev->transfer_reg, response_len);
	}

	return 0;
}

template <typename T>
T AMU::query(CMD_t command) {
	if (!amu_dev || !amu_dev->transfer_reg) {
		AMU_ERROR("Error: amu_dev or transfer_reg is null\n");
		return T{};
	}

	int8_t result = amu_dev_query_command(address, command, 0, sizeof(T));
	if (result != 0) {
		AMU_ERROR("Query command failed with error: %d\n", result);
		return T{};
	}

	T* data = (T*) amu_dev->transfer_reg;
	return *data;
}

template <typename T>
T AMU::query(CMD_t command, T* data) {
	if (!data) {
		AMU_ERROR("Error: data pointer is null\n");
		return T{};
	}

	int8_t rv = amu_dev_query_command(address, command, 0, sizeof(T));
	if (rv != 0) {
		AMU_ERROR("Query command failed with error: %d\n", rv);
		return *data;
	}

	_amu_transfer_read(0, data, sizeof(T));

	return *data;
}

template <typename T>
T * AMU::query(CMD_t command, T *data, size_t len) {
	if (!data) {
		AMU_ERROR("Error: data pointer is null\n");
		return nullptr;
	}

	if (len == 0) {
		AMU_ERROR("Error: len is zero\n");
		return data;
	}

	int8_t rv = amu_dev_query_command(address, command, 0, len);
	if (rv != 0) {
		AMU_ERROR("Query command %d failed with error: %d\n", command, rv);
		return data; // Return original pointer
	}

	_amu_transfer_read(0, data, len);


	return data;
}

template <typename T>
T AMU::queryChannel(CMD_t command, uint8_t channel) {
	if (!amu_dev || !amu_dev->transfer_reg) {
		AMU_ERROR("Error: amu_dev or transfer_reg is null\n");
		return T{};
	}

	_amu_transfer_write(0, &channel, 1);

	int8_t rv = amu_dev_query_command(address, command, 1, sizeof(T));
	if (rv != 0) {
		AMU_ERROR("Query command failed with error: %d\n", rv);
		return T{};
	}

	T* data = (T*) amu_dev->transfer_reg;
	return *data;
}

template <typename T>
T AMU::read_twi_reg(uint8_t reg) {
	T data{};

	int8_t result = amu_dev_transfer(address, reg, (uint8_t *) &data, sizeof(T), AMU_TWI_TRANSFER_READ);
	if (result != 0) {
		AMU_ERROR("Register 0x%02X read failed with error: %d\n", reg, result);

		return T{};
	}

	return data;
}

template <typename T>
T * AMU::read_twi_reg(uint8_t reg, T *data, size_t len) {
	if (!data || (len == 0)) {
		AMU_ERROR("Error: register 0x%02X read has null data or zero len\n", reg);

		return data;
	}

	int8_t result = amu_dev_transfer(address, reg, (uint8_t *) data, len, AMU_TWI_TRANSFER_READ);
	if (result != 0) {
		AMU_ERROR("Register 0x%02X read failed with error: %d\n", reg, result);
	}

	return data;
}

template <typename T>
int8_t AMU::write_twi_reg(uint8_t reg, T data) {
	return amu_dev_transfer(address, reg, (uint8_t *) &data, sizeof(T), AMU_TWI_TRANSFER_WRITE);
}

template <typename T>
int8_t AMU::write_twi_reg(uint8_t reg, T *data, size_t len) {
	return amu_dev_transfer(address, reg, (uint8_t *) data, len, AMU_TWI_TRANSFER_WRITE);
}

#endif // __AMU_REMOTE_DEVICE__

#endif // __AMU_CLASS__
