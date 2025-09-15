// amu.h

#ifndef __AMULIB_H__
#define __AMULIB_H__

#include "amulibc/amu_device.h"
#include "amulibc/amu_types.h"
#include "amulibc/amu_regs.h"
#include "amulibc/amu_commands.h"

#ifdef	__AMU_USE_SCPI__
#include "amulibc/scpi.h"
#endif

#ifdef __AMU_REMOTE_DEVICE__

#include <stdlib.h>

#ifdef __cplusplus

class AMU {

public:

	void			begin(uint8_t twiAddress);
	void			begin(uint8_t twiAddress, amu_transfer_fptr_t i2c_transfer_func);

	uint8_t			waitUntilReady(uint32_t timeout);

	int8_t			sleep(void);

	char*			readNotes(char *notes, uint8_t len);
	char*			readNotes(char* notes);
	char*			readSerialStr(void);
	char*			readFirmwareStr(void);

	int8_t			setActiveChannels(uint16_t channels);
	int8_t			setTimeStamp(uint32_t timestamp);

	int8_t			setLEDcolor(float red, float grn, float blu);
	int8_t			setLEDmode(amu_led_pattern_t mode);

	uint8_t			getPGA(AMU_ADC_CH_t channel);

	float			measureVoltage(void);
	float			measureCurrent(void);
	float			measureTSensor(void);
	float			measureTSensor0(void);
	float			measureTSensor1(void);
	float			measureTSensor2(void);
	float			measureSSTL(void);
	float			measureSSBL(void);
	float			measureSSBR(void);
	float			measureSSTR(void);
	float			measureBias(void);
	float			measureOffset(void);
	float			measureTemperature(void);
	float			measureAvdd(void);
	float			measureIOvdd(void);
	float			measureAldo(void);
	float			measureDldo(void);

	float					measureChannel(uint8_t channel);
	int8_t					measureActiveChannels(void);
	amu_int_volt_t			measureInternalVoltages(void);
	quad_photo_sensor_t		measureSunSensor(void);

	float					measureSystemTemperature(void);

	/*** EYAS FUNCTIONS ***/
	press_data_t	measurePressureSensor(void);
	float			measurePressure(void);
	float			measureHumidity(void);
	float			measurePSTemperature(void);


	ivsweep_config_t *		readSweepConfig(void);
	ivsweep_meta_t *		readMeta(void);
	float					readIsc(void);
	float					readVoc(void);
	ss_angle_t *			readSunSensorAngles(void);
	quad_photo_sensor_t	*	readSunSensorMeasurement(void);

	amu_meas_t			readMeasurement(void);
	uint32_t *			readSweepTimestamps(uint32_t*);
	float *				readSweepVoltages(float*);
	float *				readSweepCurrents(float*);
	float *				readSweepYaws(float*);
	float *				readSweepPitches(float*);
	ivsweep_packet_t *	readSweepIV(ivsweep_packet_t*);
	ivsweep_packet_t *	readSweepSunAngle(ivsweep_packet_t*);
	ivsweep_packet_t *	readSweepAll(ivsweep_packet_t*);

	void			loadSweepDatapoints(uint8_t offset);

	uint8_t			getAddress(void) { return address; }

	amu_dut_t*		getDUT(void) { return &dut; }
	char *			getDutManufacturer(void) { return dut.manufacturer; }
	char*			getDutModel(void) { return dut.model; }
	uint8_t			getDutJunction(void) { return dut.junction; }
	char *			getDutSerialNumber(void) { return dut.serial; }
	float			getDutEnergy(void) { return dut.energy; }
	float			getDutDose(void) { return dut.dose; }


	amu_hardware_revision_t getHardwareRevision(void) { return hardware_revision; }

	char *			getFirmware(void) { return firmware; }
	char*			getSerialNumber(void) { return serial_number; }

	ivsweep_config_t *	getSweepConfig(void) { return &sweep_config; }
	ivsweep_meta_t *	getMetaData(void) { return &meta; }

	float			getDACgainCorrection(void);

	quad_photo_sensor_t getQuadPhotoSensor(void) { return sun_sensor; }

	float			getPhotoDiodeVoltage(uint8_t n);
	float			getYaw(void) { return sun_sensor.angle.yaw; }
	float			getPitch(void) { return sun_sensor.angle.pitch; }

	float			getYawAbs(void) { return abs(sun_sensor.angle.yaw); }
	float			getPitchAbs(void) { return abs(sun_sensor.angle.pitch); }

	bool			goodSunAngle(float minAngle);

	amu_coeff_t		getYawCoefficients(void);
	amu_coeff_t		getPitchCoefficients(void);
	float			getSSHVal(void);
	float			getSSRVal(void);

	uint32_t		getADCstatus(void);

	int8_t			triggerIsc(void);
	int8_t			triggerVoc(void);

	int8_t			triggerSweep(void);

	amu_meas_t		measureIsc(void);
	amu_meas_t		measureVoc(void);



	

public:

		typedef void (*errorPrintFncPtr_t)(const char* fmt, ...);
		typedef void (*resetFncPtr_t)(void);

		static errorPrintFncPtr_t errorPrintFncPtr;
		static resetFncPtr_t amuResetFncPtr;
		static resetFncPtr_t eyasResetFncPtr;

		static amu_device_t *	amu_lib_init(amu_transfer_fptr_t i2c_transfer_func);
		static amu_scpi_dev_t * amu_scpi_init(size_t(*write_cmd)(const char* data, size_t len), void(*flush_cmd)(void));

public:

	static void setErrorPrintFunction(errorPrintFncPtr_t fptr) { errorPrintFncPtr = fptr; }
	static void	setAMUResetFuncPtr(resetFncPtr_t fptr) { amuResetFncPtr = fptr; }
	static void	setEYASResetFuncPtr(resetFncPtr_t fptr) { eyasResetFncPtr = fptr; }

protected:

	uint8_t address;

	volatile amu_device_t* amu_dev;

	char serial_number[AMU_SERIALNUM_STR_LEN];
	char firmware[AMU_FIRMWARE_STR_LEN];

	amu_hardware_revision_t hardware_revision;

	amu_dut_t dut;

	ivsweep_config_t sweep_config;
	ivsweep_meta_t meta;

	quad_photo_sensor_t sun_sensor;

	uint8_t		busy(void);

	int8_t		sendCommand(CMD_t cmd);
	int8_t		sendCommand(CMD_t cmd, void* params, uint8_t param_len);
	int8_t		sendCommandandWait(CMD_t cmd, uint32_t wait);

	template <typename T>
	T query(CMD_t command);

	template <typename T>
	T query(CMD_t command, T* data);

	template <typename T>
	T * query(CMD_t command, T *data, size_t len);

	template <typename T>
	T queryChannel(CMD_t command, uint8_t channel);

	template <typename T>
	T read_twi_reg(uint8_t reg);

	template <typename T>
	T * read_twi_reg(uint8_t reg, T *data, size_t len);

	template <typename T>
	int8_t write_twi_reg(uint8_t reg, T data);

	template <typename T>
	int8_t write_twi_reg(uint8_t reg, T * data, size_t len);
};

#endif // __cplusplus

#endif // __AMU_REMOTE_DEVICE__

#endif	/* __AMULIB_H__ */
