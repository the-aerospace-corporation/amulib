// 
// 
// 
#ifdef __AMU_REMOTE_DEVICE__

#include "amulib.h"

#ifdef ARDUINO
#include <Arduino.h>
#endif

AMU::errorPrintFncPtr_t AMU::errorPrintFncPtr = nullptr;
AMU::resetFncPtr_t AMU::amuResetFncPtr = nullptr;
AMU::resetFncPtr_t AMU::eyasResetFncPtr = nullptr;

void AMU::begin(uint8_t twiAddress) {

	address = twiAddress;

	readFirmwareStr();

	readSerialStr();

	hardware_revision = (amu_hardware_revision_t)read_twi_reg< uint8_t >(AMU_REG_SYSTEM_HARDWARE_REVISION);

	dut = read_twi_reg<amu_dut_t>(AMU_REG_DUT);

	readSweepConfig();
}

void AMU::begin(uint8_t twiAddress, amu_transfer_fptr_t i2c_transfer_func) {

	amu_dev = amu_lib_init(i2c_transfer_func);

	begin(twiAddress);

}

uint8_t AMU::waitUntilReady(uint32_t timeout) {

	if (!amu_dev->millis)
		return 2;

	uint32_t waitTime = amu_dev->millis();

	while(busy() && ((amu_dev->millis() - waitTime) < timeout)) {
		if (amu_dev->delay)
			amu_dev->delay(3);
	}

	if ((amu_dev->millis() -waitTime) > timeout)
		return 1;
	else
		return 0;
}

int8_t AMU::sleep(void) {
	return sendCommand((CMD_t)CMD_SYSTEM_SLEEP);
}

char* AMU::readNotes(char* notes, uint8_t len) {
	return query<char>((CMD_t)CMD_DUT_NOTES, notes, len);
}

char* AMU::readNotes(char* notes) {
	return query<char>((CMD_t)CMD_DUT_NOTES, notes, AMU_NOTES_SIZE);
}

char* AMU::readSerialStr() {
	return query<char>((CMD_t)CMD_SYSTEM_SERIAL_NUM, (char*)&serial_number, (size_t)AMU_SERIALNUM_STR_LEN);
}

char* AMU::readFirmwareStr() {
	return query<char>((CMD_t)CMD_SYSTEM_FIRMWARE, (char*)&firmware, (size_t)AMU_FIRMWARE_STR_LEN);
}

int8_t AMU::setActiveChannels(uint16_t channels) {
	return write_twi_reg<uint16_t>(AMU_REG_SYSTEM_ADC_ACTIVE_CHANNELS, channels);
}

int8_t AMU::setTimeStamp(uint32_t timestamp) {
	//SAFE_CMD_DATA(CMD_SYSTEM_TIME, &timestamp, sizeof(uint32_t), "CMD TIMESTAMP");
	return 0;
}

int8_t AMU::setLEDcolor(float red, float grn, float blu) {
	amu_pid_t colors = { red, grn, blu };
	return sendCommand((CMD_t)CMD_SYSTEM_LED_COLOR, (void *)&colors, sizeof(amu_pid_t));
}

int8_t AMU::setLEDmode(amu_led_pattern_t mode) {
	return sendCommand((CMD_t)(CMD_SYSTEM_LED + mode));
}

uint8_t AMU::getPGA(AMU_ADC_CH_t channel) {
	if (channel < AMU_ADC_CH_NUM)
		return (1<<queryChannel<uint8_t>((CMD_t)CMD_ADC_CH_PGA, channel));
	else
		return 0;
}

float AMU::measureVoltage() { return query<float>((CMD_t)CMD_MEAS_CH_VOLTAGE);	}
float AMU::measureCurrent() { return query<float>((CMD_t)CMD_MEAS_CH_CURRENT); }
float AMU::measureTSensor() { return query<float>((CMD_t)CMD_MEAS_CH_TSENSOR_0); }
float AMU::measureTSensor0() { return query<float>((CMD_t)CMD_MEAS_CH_TSENSOR_0); }
float AMU::measureTSensor1() { return query<float>((CMD_t)CMD_MEAS_CH_TSENSOR_1); }
float AMU::measureTSensor2() { return query<float>((CMD_t)CMD_MEAS_CH_TSENSOR_2); }
float AMU::measureSSTL() { return query<float>((CMD_t)CMD_MEAS_CH_SS_TL); }
float AMU::measureSSBL() { return query<float>((CMD_t)CMD_MEAS_CH_SS_BL); }
float AMU::measureSSBR() { return query<float>((CMD_t)CMD_MEAS_CH_SS_BR); }
float AMU::measureSSTR() { return query<float>((CMD_t)CMD_MEAS_CH_SS_TR); }
float AMU::measureBias() { return query<float>((CMD_t)CMD_MEAS_CH_BIAS); }
float AMU::measureOffset() { return query<float>((CMD_t)CMD_MEAS_CH_OFFSET); }
float AMU::measureTemperature() { return query<float>((CMD_t)CMD_MEAS_CH_TEMP); }
float AMU::measureAvdd() { return query<float>((CMD_t)CMD_MEAS_CH_AVDD); }
float AMU::measureIOvdd() { return query<float>((CMD_t)CMD_MEAS_CH_IOVDD); }
float AMU::measureAldo() { return query<float>((CMD_t)CMD_MEAS_CH_ALDO); }
float AMU::measureDldo() { return query<float>((CMD_t)CMD_MEAS_CH_DLDO); }

float AMU::measureChannel(uint8_t channel) {
	if (channel < AMU_ADC_CH_NUM) {
		return query<float>((CMD_t)(CMD_MEAS_CH_VOLTAGE + channel));
	}
	else
		return -1.000;
}

int8_t AMU::measureActiveChannels() {
	return sendCommand((CMD_t)CMD_EXEC_MEAS_ACTIVE_CHANNELS);
}

amu_int_volt_t AMU::measureInternalVoltages(void) {
	return query<amu_int_volt_t>((CMD_t)CMD_EXEC_MEAS_INTERNAL_VOLTAGES);
}

press_data_t AMU::measurePressureSensor(void) {
	return query<press_data_t>((CMD_t)CMD_EXEC_MEAS_PRESSURE_SENSOR);
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

//TYPE data; SAFE_CMD(_queryCommand((CMD_t)((X) | CMD_READ), 0, sizeof(TYPE)), ERROR_MSG); _amu_transfer_read(0, &data, sizeof(TYPE)); return data;
quad_photo_sensor_t AMU::measureSunSensor() {
	return query<quad_photo_sensor_t>((CMD_t)CMD_EXEC_MEAS_SUN_SENSOR, &sun_sensor);
}

ivsweep_config_t * AMU::readSweepConfig() { sweep_config = read_twi_reg<ivsweep_config_t>(AMU_REG_DATA_PTR_SWEEP_CONFIG); return &sweep_config; }
ivsweep_meta_t * AMU::readMeta() { meta = read_twi_reg<ivsweep_meta_t>(AMU_REG_DATA_PTR_SWEEP_META); return &meta; }

float AMU::readIsc() { meta.isc = read_twi_reg<float>(AMU_REG_SWEEP_META_ISC); return meta.isc; }
float AMU::readVoc() { meta.voc = read_twi_reg<float>(AMU_REG_SWEEP_META_VOC); return meta.voc; }

ss_angle_t * AMU::readSunSensorAngles() { sun_sensor.angle = read_twi_reg<ss_angle_t>(AMU_REG_SUNSENSOR_ANGLE); return &sun_sensor.angle; }
quad_photo_sensor_t * AMU::readSunSensorMeasurement() { sun_sensor = read_twi_reg<quad_photo_sensor_t>(AMU_REG_SUNSENSOR); return &sun_sensor; }

amu_meas_t AMU::readMeasurement(void) { return read_twi_reg<amu_meas_t>(AMU_REG_TRANSFER_PTR); }

uint32_t * AMU::readSweepTimestamps(uint32_t* data) { return read_twi_reg<uint32_t>(AMU_REG_DATA_PTR_TIMESTAMP, data, sizeof(uint32_t) * sweep_config.numPoints); }
float * AMU::readSweepVoltages(float* data) { return read_twi_reg<float>(AMU_REG_DATA_PTR_VOLTAGE, data, sizeof(float) * sweep_config.numPoints); }
float * AMU::readSweepCurrents(float* data) { return read_twi_reg<float>(AMU_REG_DATA_PTR_CURRENT, data, sizeof(float) * sweep_config.numPoints); }
float* AMU::readSweepYaws(float* data) { return read_twi_reg<float>(AMU_REG_DATA_PTR_SS_YAW, data, sizeof(float) * sweep_config.numPoints); }
float* AMU::readSweepPitches(float* data) { return read_twi_reg<float>(AMU_REG_DATA_PTR_SS_PITCH, data, sizeof(float) * sweep_config.numPoints); }


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
	readSweepYaws(sweep_packet->voltage);
	readSweepPitches(sweep_packet->current);
#endif
	return sweep_packet;
}

ivsweep_packet_t* AMU::readSweepAll(ivsweep_packet_t* sweep_packet) {
	readSweepIV(sweep_packet);
#ifndef __AMU_LOW_MEMORY__
	readSweepSunAngle(sweep_packet);
#endif
	return (ivsweep_packet_t*)amu_dev->sweep_data;
}

void AMU::loadSweepDatapoints(uint8_t offset) {
	sendCommand((CMD_t)CMD_SWEEP_DATAPOINT_LOAD, &offset, 1);
}

bool AMU::goodSunAngle(float minAngle) {
	if ((getYaw() == NAN) || (getPitch() == NAN))		// test this?
		return false;

	return ((getYawAbs() < minAngle) && (getPitchAbs() < minAngle));
}

float AMU::getPhotoDiodeVoltage(uint8_t n) {
	if (n < 4)
		return sun_sensor.diode[n];
	else
		return -1.000;
}

float AMU::getDACgainCorrection(void) {
	return query<float>((CMD_t)CMD_AUX_DAC_GAIN_CORRECTION);
}

amu_coeff_t AMU::getYawCoefficients(void) { return query<amu_coeff_t>((CMD_t)CMD_AUX_SUNSENSOR_FIT_YAW_COEFF); }
amu_coeff_t AMU::getPitchCoefficients(void) { return query<amu_coeff_t>((CMD_t)CMD_AUX_SUNSENSOR_FIT_PITCH_COEFF); }

float AMU::getSSHVal(void) { return query<float>((CMD_t)CMD_AUX_SUNSENSOR_HVAL); }
float AMU::getSSRVal(void) { return query<float>((CMD_t)CMD_AUX_SUNSENSOR_RVAL); }

uint32_t AMU::getADCstatus(void) {
	return read_twi_reg<uint32_t>((uint8_t)AMU_REG_SYSTEM_STATUS_HRADC);
}

int8_t AMU::triggerIsc(void) {
	return sendCommand((CMD_t)CMD_SWEEP_TRIG_ISC);
}

int8_t AMU::triggerVoc(void) {
	return sendCommand((CMD_t)CMD_SWEEP_TRIG_VOC);
}

int8_t AMU::triggerSweep(void) {
	return sendCommand((CMD_t)CMD_SWEEP_TRIG_SWEEP);
}

amu_meas_t AMU::measureIsc(void) {
	return query<amu_meas_t>((CMD_t)CMD_SWEEP_TRIG_ISC);
}

amu_meas_t AMU::measureVoc(void) {
	return query<amu_meas_t>((CMD_t)CMD_SWEEP_TRIG_VOC);
}

uint8_t AMU::busy() {
	return amu_dev_busy(address);
}

//// Private functions, do not use externally by any means, only use SAFE_CMD/QUERY
int8_t AMU::sendCommand(CMD_t cmd) {
	static uint8_t wait_error = 0;

	wait_error = waitUntilReady(1000);

	if (wait_error != 0) {
		if (AMU::errorPrintFncPtr) {
			AMU::errorPrintFncPtr("Wait until ready error: %u\n", wait_error);
		}
	}
	return amu_dev_send_command(address, cmd);
}

int8_t AMU::sendCommand(CMD_t cmd, void *params, uint8_t param_len) {
	amu_dev_transfer(address, AMU_REG_TRANSFER_PTR, (uint8_t*)params, param_len, AMU_TWI_TRANSFER_WRITE);
	return sendCommand(cmd);
}

int8_t AMU::sendCommandandWait(CMD_t cmd, uint32_t wait) {
	sendCommand(cmd);
	return waitUntilReady(wait);
}

template <typename T>
T AMU::query(CMD_t command) {
	if (!amu_dev || !amu_dev->transfer_reg) {
		if (AMU::errorPrintFncPtr) {
			AMU::errorPrintFncPtr("Error: amu_dev or transfer_reg is null\n");
		}
		return T{}; // Return default-constructed value
	}
	
	int8_t result = amu_dev_query_command(address, command, 0, sizeof(T));
	if (result < 0) {
		if (AMU::errorPrintFncPtr) {
			AMU::errorPrintFncPtr("Query command failed with error: %d\n", result);
		}
		return T{};
	}
	
	T* data = (T*)amu_dev->transfer_reg;
	return *data;
}

template <typename T>
T AMU::query(CMD_t command, T* data) {
	if (!data) {
		if (AMU::errorPrintFncPtr) {
			AMU::errorPrintFncPtr("Error: data pointer is null\n");
		}
		return T{};
	}
	
	int8_t result = amu_dev_query_command(address, command, 0, sizeof(T));
	if (result < 0) {
		if (AMU::errorPrintFncPtr) {
			AMU::errorPrintFncPtr("Query command failed with error: %d\n", result);
		}
		return *data; // Return existing data value
	}
	
	_amu_transfer_read(0, data, sizeof(T));
	
	return *data;
}

template <typename T>
T * AMU::query(CMD_t command, T *data, size_t len) {
	if (!data) {
		if (AMU::errorPrintFncPtr) {
			AMU::errorPrintFncPtr("Error: data pointer is null\n");
		}
		return nullptr;
	}
	
	if (len == 0) {
		if (AMU::errorPrintFncPtr) {
			AMU::errorPrintFncPtr("Error: len is zero\n");
		}
		return data;
	}
	
	int8_t result = amu_dev_query_command(address, command, 0, len);
	if (result < 0) {
		if (AMU::errorPrintFncPtr) {
			AMU::errorPrintFncPtr("Query command failed with error: %d\n", result);
		}
		return data; // Return original pointer
	}
	
	_amu_transfer_read(0, data, len);

	
	return data;
}

template <typename T>
T AMU::queryChannel(CMD_t command, uint8_t channel) {
	if (!amu_dev || !amu_dev->transfer_reg) {
		if (AMU::errorPrintFncPtr) {
			AMU::errorPrintFncPtr("Error: amu_dev or transfer_reg is null\n");
		}
		return T{};
	}
	
	_amu_transfer_write(0, &channel, 1);
	
	int8_t result = amu_dev_query_command(address, command, 0, sizeof(T));
	if (result != 0) {
		if (AMU::errorPrintFncPtr) {
			AMU::errorPrintFncPtr("Query command failed with error: %d\n", result);
		}
		return T{};
	}
	
	T* data = (T*)amu_dev->transfer_reg;
	return *data;
}

template <typename T>
T AMU::read_twi_reg(uint8_t reg) {
	T data;
	amu_dev_transfer(address, reg, (uint8_t *)&data, sizeof(T), AMU_TWI_TRANSFER_READ);
	return data;
}

template <typename T>
T * AMU::read_twi_reg(uint8_t reg, T *data, size_t len) {
	amu_dev_transfer(address, reg, (uint8_t *)data, len, AMU_TWI_TRANSFER_READ);
	return data;
}

template <typename T>
int8_t AMU::write_twi_reg(uint8_t reg, T data) {
	return amu_dev_transfer(address, reg, (uint8_t *)&data, sizeof(T), AMU_TWI_TRANSFER_WRITE);
}

template <typename T>
int8_t AMU::write_twi_reg(uint8_t reg, T *data, size_t len) {
	return amu_dev_transfer(address, reg, (uint8_t *)data, len, AMU_TWI_TRANSFER_WRITE);
}

amu_device_t* AMU::amu_lib_init(amu_transfer_fptr_t i2c_transfer_func) {

	amu_device_t * dev = (amu_device_t * )amu_dev_init(i2c_transfer_func);

	dev->delay = delay;		// delay function pointer amu_delay_fptr_t
	dev->millis = millis;	// millis fucnction pointer amu_milis_fptr_t

	return dev;
}

amu_scpi_dev_t* AMU::amu_scpi_init(size_t(*write_cmd)(const char* data, size_t len), void(*flush_cmd)(void)) {
#ifdef __AMU_USE_SCPI__
	amu_scpi_dev_t* scpi_dev = (amu_scpi_dev_t *)amu_get_scpi_dev();
	if (scpi_dev != NULL) {
		scpi_dev->write_cmd = write_cmd;
		scpi_dev->flush_cmd = flush_cmd;
	}
	return scpi_dev;
#else
	return nullptr;
#endif
}

#endif