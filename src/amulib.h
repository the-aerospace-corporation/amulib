/**
 * @file amulib.h
 * @brief C++ class layer over amulibc: the AMU class and its remote-device measurement API
 */
#ifndef __AMULIB_H__
#define __AMULIB_H__

#include "amulibc/amu_device.h"
#include "amulibc/amu_regs.h"
#include "amulibc/amu_config_internal.h"

#ifdef	__AMU_USE_SCPI__
#include "amulibc/scpi.h"
#endif

#ifdef ARDUINO
#include <Arduino.h>
#include <Wire.h>
#endif // ARDUINO

#ifdef __AMU_REMOTE_DEVICE__
#include <stdlib.h>
#endif

#if defined(__cplusplus) && (defined(ARDUINO) || defined(__AMU_REMOTE_DEVICE__))
#define __AMU_CLASS__
#endif

#ifdef __AMU_CLASS__

class AMU {

public:
#ifdef ARDUINO
	/**
	 * @brief Constructs an AMU and registers the Wire transfer callback
	 * @param wire TWI bus to drive, defaults to Wire
	 */
	AMU(TwoWire* wire = &Wire);
#else
	AMU(void);
#endif // ARDUINO

#ifdef __AMU_REMOTE_DEVICE__
	/**
	 * @brief Constructs an AMU driven by a caller-supplied transfer callback
	 * @param i2c_transfer_func Platform TWI read/write callback
	 */
	AMU(amu_transfer_fptr_t i2c_transfer_func);
#endif // __AMU_REMOTE_DEVICE__

	typedef void (*errorPrintFncPtr_t)(const char* fmt, ...);
	typedef void (*resetFncPtr_t)(void);

	/**
	 * @brief Initializes the shared device layer with a transfer callback
	 *
	 * @param i2c_transfer_func Platform TWI read/write callback
	 * @return Pointer to the device singleton
	 */
	static amu_device_t *		amu_lib_init(amu_transfer_fptr_t i2c_transfer_func);

#ifdef __AMU_USE_SCPI__
	/**
	 * @brief Installs the SCPI transport I/O callbacks
	 *
	 * @param write_cmd Writes SCPI output to the transport
	 * @param flush_cmd Flushes the transport
	 * @return Pointer to the SCPI device
	 */
	static amu_scpi_dev_t *		amu_scpi_init(size_t(*write_cmd)(const char* data, size_t len), void(*flush_cmd)(void));
#endif // __AMU_USE_SCPI__

	static void					setErrorPrintFunction(errorPrintFncPtr_t fptr) { errorPrintFncPtr = fptr; }
	static void					setAMUResetFuncPtr(resetFncPtr_t fptr) { amuResetFncPtr = fptr; }
	static void					setEYASResetFuncPtr(resetFncPtr_t fptr) { eyasResetFncPtr = fptr; }

#ifdef ARDUINO
	static int8_t				wireTransfer(TwoWire* wire, uint8_t address, uint8_t reg, uint8_t* data, size_t len, uint8_t read);
	static int8_t				twiTransfer(uint8_t address, uint8_t reg, uint8_t* data, size_t len, uint8_t read);

	/**
	 * @brief Selects the TWI bus and initializes the device layer on it
	 *
	 * @param wire TWI bus to drive, defaults to Wire
	 * @return Pointer to the device singleton
	 */
	static amu_device_t*		devInit(TwoWire* wire = &Wire);

	/**
	 * @brief Initializes the device layer with a caller-supplied callback, adding Arduino delay/millis
	 *
	 * @param i2c_transfer_func Platform TWI read/write callback
	 * @return Pointer to the device singleton
	 */
	static amu_device_t*		devInit(amu_transfer_fptr_t i2c_transfer_func);

	static void					usbInit(Stream& stream);
	static size_t				usbWrite(const char* data, size_t len);
	static void					usbFlush(void);

#ifdef __AMU_USE_SCPI__
	/**
	 * @brief Reads one character off the selected stream, echoing and buffering until a
	 *        terminator completes a SCPI line
	 *
	 * @details Non-blocking; call it every loop().
	 */
	static void					scpiProcessStream(void);

	/**
	 * @brief Selects the stream, then processes it
	 *
	 * @details Selecting here -> echo and SCPI responses cannot end up on different streams.
	 *
	 * @param stream Stream carrying SCPI traffic
	 */
	static void					scpiProcessStream(Stream& stream);
#endif // __AMU_USE_SCPI__

#endif // ARDUINO

#ifdef __AMU_REMOTE_DEVICE__

	/**
	 * @brief Binds this instance to an AMU and caches its identity, DUT record and sweep config
	 *
	 * @param twiAddress Address of the AMU, e.g. from amu_get_device_address()
	 */
	void						begin(uint8_t twiAddress);

	/**
	 * @brief Initializes the device layer with a transfer callback, then binds to an AMU
	 *
	 * @param twiAddress		Address of the AMU
	 * @param i2c_transfer_func	Platform TWI read/write callback
	 */
	void						begin(uint8_t twiAddress, amu_transfer_fptr_t i2c_transfer_func);

	/**
	 * @brief Polls the device until it reports not busy
	 *
	 * @param timeout Milliseconds to keep polling
	 * @return 0 once ready, nonzero on timeout or TWI error
	 */
	int8_t						waitUntilReady(uint32_t timeout);

	int8_t						sleep(void);				///< Puts the AMU to sleep

	/**
	 * @brief Restarts the AMU
	 *
	 * @details If the running image is uncommitted this is also how it is rolled back - the
	 * bootloader swaps in the image it replaced on the way up. That is the way out of an image
	 * that refuses to be replaced without committing one you did not want.
	 *
	 * @return 0 on success, nonzero TWI error otherwise
	 */
	int8_t						reset(void);

	/**
	 * @brief Reads the DUT notes field
	 *
	 * @param notes	Destination buffer
	 * @param len	Bytes to read
	 * @return notes
	 */
	char*						readNotes(char *notes, uint8_t len);
	char*						readNotes(char* notes);

	char*						readSerialStr(void);
	char*						readFirmwareStr(void);

	uint8_t						getAddress(void) { return address; }
	char *						getFirmware(void) { return firmware; }
	char*						getSerialNumber(void) { return serial_number; }

	/** @brief Cached hardware revision, BCD-encoded: high nibble major, low nibble minor */
	amu_hardware_revision_t		getHardwareRevision(void) { return hardware_revision; }

	amu_dut_t*					getDUT(void) { return &dut; }
	char *						getDutManufacturer(void) { return dut.manufacturer; }
	char*						getDutModel(void) { return dut.model; }
	uint8_t						getDutJunction(void) { return dut.junction; }
	char *						getDutSerialNumber(void) { return dut.serial; }
	float						getDutEnergy(void) { return dut.energy; }
	float						getDutDose(void) { return dut.dose; }
	uint8_t						getDutCoverglass(void) { return dut.coverglass; }
	uint8_t						getDutInterconnect(void) { return dut.interconnect; }
	char*						getDutTechnology(void) { return dut.technology; }

	/** @brief Live query - tsensor_type/tsensor_num/tsensor fit coefficients are not cached in begin() */
	uint8_t						getDutTsensorType(void);
	uint8_t						getDutTsensorNumber(void);
	amu_coeff_t					getDutTsensorFit(void);

	// -----------------------------------------------------------------------
	// Device configuration
	// -----------------------------------------------------------------------

	/**
	 * @brief Sets which ADC channels measureActiveChannels() sweeps
	 *
	 * @param channels Bitmask of AMU_CH_EN_t values
	 * @return 0 on success, nonzero TWI error otherwise
	 */
	int8_t						setActiveChannels(uint16_t channels);

	/** @brief Sets the AMU's UTC clock, which timestamps sweep datapoints */
	int8_t						setTimeStamp(uint32_t timestamp);

	int8_t						setLEDcolor(float red, float grn, float blu);
	int8_t						setLEDmode(amu_led_pattern_t mode);

	/**
	 * @brief Enables or disables the AMU's BLE radio
	 *
	 * The setting is stored in non-volatile memory and reapplied on boot. Defaults to off.
	 *
	 * @param state true to advertise, false to shut the radio down
	 * @return 0 on success, nonzero TWI error otherwise
	 */
	/** @brief Enables or disables the DAC output driving the DUT */
	int8_t						setDACState(bool state);
	bool						getDACState(void);

	int8_t						setDACVoltage(float voltage);
	float						getDACVoltage(void);

	/** @brief Enables or disables the DUT temperature-control heater */
	int8_t						setHeaterState(bool state);
	bool						getHeaterState(void);

	int8_t						enableBluetooth(bool state);

	bool						bluetoothEnabled(void);		///< True while the BLE radio is advertising

	/**
	 * @brief Pins the AMU's BLE pairing passkey, or restores a random one per pairing
	 *
	 * The setting is stored in non-volatile memory and reapplied on boot. Takes effect on
	 * the next pairing; an established bond is unaffected.
	 *
	 * @param passkey 1 to 999999 to pin, 0 to unpin
	 * @return 0 on success, nonzero TWI error otherwise
	 * @warning A pinned passkey does not rotate. Where this master can reach the device,
	 *          bluetoothPasskey() reads the random one instead and gives up nothing.
	 */
	int8_t						setBluetoothPasskey(uint32_t passkey);

	/**
	 * @brief Reads the passkey the AMU's next or in-flight pairing will use
	 *
	 * The device pairs DisplayOnly, so an unpinned passkey exists only for the duration of
	 * the exchange. This is how a master pairs a new host to a device whose console it
	 * cannot see: start pairing from the new host, read the passkey here, enter it there.
	 *
	 * @return Pinned passkey, else the last one a pairing drew, else 0
	 */
	uint32_t					bluetoothPasskey(void);

	/**
	 * @brief Opens a firmware transfer, erasing the device's spare application partition
	 *
	 * @details The staged image is verified at updateEnd() against the SHA-256 it carries, so a
	 * byte lost anywhere in the chain fails the update rather than booting. A device whose running
	 * image is still uncommitted refuses to stage another - commit that one first.
	 *
	 * @param size Image size in bytes
	 * @return Largest chunk the device will accept, or 0 if it refused
	 */
	uint32_t					updateBegin(uint32_t size);

	/**
	 * @brief Appends the next sequential chunk of the image
	 *
	 * @details The reply is the flow control: the device answers only once the chunk is in flash,
	 * so a caller that waits for it cannot outrun the writes.
	 *
	 * @param data Chunk bytes, at most the chunk length updateBegin() returned
	 * @param len Number of bytes
	 * @return Bytes the device has written so far, to be compared against the total sent
	 */
	uint32_t					updateData(const void* data, uint16_t len);

	/**
	 * @brief Verifies the image, sets it as the boot partition and restarts the device
	 *
	 * @details The device comes back uncommitted. Confirm it works and call updateCommit(), or
	 * the next reset rolls it back to the image it replaced.
	 *
	 * @return 0 on success
	 */
	int32_t						updateEnd(void);

	/**
	 * @brief Reads the device's view of a transfer - where it got to, and whether it failed
	 *
	 * @details Any pointer may be NULL. @p written and @p total mean nothing unless @p state is
	 * AMU_FW_STATE_RECEIVING or AMU_FW_STATE_FAILED, where they say how far a push got.
	 *
	 * Call this before a transfer or after one fails, not during: the device stages the response
	 * in the transfer register, which is the same register chunks are written into.
	 *
	 * A host resuming an interrupted push continues from @p written - the device's own count, not
	 * its own. A chunk the device wrote but never got to answer for is already there, and counting
	 * it again would send it twice.
	 *
	 * @return false if the device could not be reached
	 */
	bool						updateState(uint8_t *state, uint32_t *written, uint32_t *total, uint32_t *chunk);

	int8_t						updateAbort(void);			///< Discards a transfer in progress
	int8_t						updateCommit(void);			///< Confirms the running image, cancelling rollback
	bool						updateCommitted(void);		///< False while the running image is uncommitted

	// -----------------------------------------------------------------------
	// ADC measurements
	//
	// Each triggers a fresh conversion on the AMU and blocks for the result.
	// -----------------------------------------------------------------------

	float						measureVoltage(void);		///< DUT voltage (V)
	float						measureCurrent(void);		///< DUT current (A)
	float						measureTSensor(void);		///< Temperature sensor 0 (C)
	float						measureTSensor0(void);		///< Temperature sensor 0 (C)
	float						measureTSensor1(void);		///< Temperature sensor 1 (C)
	float						measureTSensor2(void);		///< Temperature sensor 2 (C)
	float						measureSSTL(void);			///< Sun sensor top-left photodiode (V)
	float						measureSSBL(void);			///< Sun sensor bottom-left photodiode (V)
	float						measureSSBR(void);			///< Sun sensor bottom-right photodiode (V)
	float						measureSSTR(void);			///< Sun sensor top-right photodiode (V)
	float						measureBias(void);			///< DAC bias rail (V)
	float						measureOffset(void);		///< Measurement offset (V)
	float						measureTemperature(void);	///< ADC die temperature (C)
	float						measureAvdd(void);			///< AVDD rail (V)
	float						measureIOvdd(void);			///< IOVDD rail (V)
	float						measureAldo(void);			///< Analog LDO (V)
	float						measureDldo(void);			///< Digital LDO (V)

	/**
	 * @brief Measures one ADC channel by index
	 *
	 * @param channel AMU_ADC_CH_t channel index
	 * @return Channel reading, or -1.0 if @p channel is out of range
	 */
	float						measureChannel(uint8_t channel);

	/** @brief Measures every channel enabled by setActiveChannels() */
	int8_t						measureActiveChannels(void);

	amu_int_volt_t				measureInternalVoltages(void);	///< AVDD, IOVDD, ALDO and DLDO in one query
	float						measureSystemTemperature(void);	///< MCU die temperature (C)

	/**
	 * @brief Reads a channel's programmable gain
	 *
	 * @param channel ADC channel
	 * @return Gain as a multiplier (1, 2, 4...), or 0 if @p channel is out of range
	 */
	uint8_t						getPGA(AMU_ADC_CH_t channel);

	uint32_t					getADCstatus(void);			///< Raw high-resolution ADC status register

	// -----------------------------------------------------------------------
	// EYAS pressure sensor
	// -----------------------------------------------------------------------

	press_data_t				measurePressureSensor(void);	///< Pressure, humidity and temperature in one query
	float						measurePressure(void);			///< Pressure only; costs a full sensor query
	float						measureHumidity(void);			///< Humidity only; costs a full sensor query
	float						measurePSTemperature(void);		///< Sensor temperature only; costs a full sensor query

	// -----------------------------------------------------------------------
	// Sun sensor
	// -----------------------------------------------------------------------

	/** @brief Measures all four photodiodes and caches the result */
	quad_photo_sensor_t			measureSunSensor(void);

	/** @brief Reads the AMU's computed yaw/pitch into the cache */
	ss_angle_t *				readSunSensorAngles(void);

	/** @brief Measures the photodiodes and returns the cached sensor record */
	quad_photo_sensor_t	*		readSunSensorMeasurement(void);

	quad_photo_sensor_t			getQuadPhotoSensor(void) { return sun_sensor; }

	/**
	 * @brief Cached voltage of one photodiode
	 *
	 * @param n Diode index 0-3
	 * @return Diode voltage, or -1.0 if @p n is out of range
	 */
	float						getPhotoDiodeVoltage(uint8_t n);

	float						getYaw(void) { return sun_sensor.angle.yaw; }
	float						getPitch(void) { return sun_sensor.angle.pitch; }

	float						getYawAbs(void) { return abs(sun_sensor.angle.yaw); }
	float						getPitchAbs(void) { return abs(sun_sensor.angle.pitch); }

	/**
	 * @brief Tests the cached sun angle against a limit
	 *
	 * @param minAngle Limit in degrees, applied to both axes
	 * @return true when both axes are inside the limit; false if either is NaN
	 */
	bool						goodSunAngle(float minAngle);

	// -----------------------------------------------------------------------
	// Aux: calibration coefficients
	// -----------------------------------------------------------------------

	float						getDACgainCorrection(void);
	amu_coeff_t					getYawCoefficients(void);
	amu_coeff_t					getPitchCoefficients(void);
	float						getSSHVal(void);
	float						getSSRVal(void);

	/** @brief Minimum aperture sum for a sun-angle reading to be considered valid */
	int8_t						setSSThreshold(float threshold);
	float						getSSThreshold(void);

	// -----------------------------------------------------------------------
	// IV sweep: triggers
	//
	// trigger* return as soon as the command is sent -> follow with waitUntilReady().
	// measure* block and hand back the datapoint.
	// -----------------------------------------------------------------------

	int8_t						triggerIsc(void);
	int8_t						triggerVoc(void);
	int8_t						triggerSweep(void);

	amu_meas_t					measureIsc(void);
	amu_meas_t					measureVoc(void);

	// -----------------------------------------------------------------------
	// IV sweep: results
	//
	// The read* calls fetch from the AMU; the get* calls hand back what the last
	// read (or begin()) cached, without touching the bus.
	// -----------------------------------------------------------------------

	amu_meas_t					readMeasurement(void);		///< Last datapoint staged in the transfer register
	ivsweep_config_t *			readSweepConfig(void);
	ivsweep_meta_t *			readMeta(void);
	float						readIsc(void);
	float						readVoc(void);

	ivsweep_config_t *			getSweepConfig(void) { return &sweep_config; }
	ivsweep_meta_t *			getMetaData(void) { return &meta; }

	/**
	 * @brief Reads one sweep array off the AMU
	 *
	 * @details Reads getSweepConfig()->numPoints elements, clamped to IVSWEEP_MAX_POINTS,
	 *          so the destination must hold at least that many.
	 *
	 * @param data Destination buffer
	 * @return data
	 */
	uint32_t *					readSweepTimestamps(uint32_t*);
	float *						readSweepVoltages(float*);	///< @see readSweepTimestamps() for the buffer size contract
	float *						readSweepCurrents(float*);	///< @see readSweepTimestamps() for the buffer size contract
	float *						readSweepYaws(float*);		///< @see readSweepTimestamps() for the buffer size contract
	float *						readSweepPitches(float*);	///< @see readSweepTimestamps() for the buffer size contract

	ivsweep_packet_t *			readSweepIV(ivsweep_packet_t*);			///< Fills the packet's voltage and current arrays

	/** @brief Fills the packet's yaw and pitch arrays; reports an error under __AMU_LOW_MEMORY__ */
	ivsweep_packet_t *			readSweepSunAngle(ivsweep_packet_t*);

	/** @brief readSweepIV() plus readSweepSunAngle(), the latter skipped under __AMU_LOW_MEMORY__ */
	ivsweep_packet_t *			readSweepAll(ivsweep_packet_t*);

	/**
	 * @brief Stages 10 sweep datapoints in the transfer register for a direct bus read
	 *
	 * @details Lets memory-constrained boards walk the curve without buffering it.
	 *
	 * @param offset Index of the first datapoint to stage
	 */
	void						loadSweepDatapoints(uint8_t offset);

#endif // __AMU_REMOTE_DEVICE__

protected:
	static errorPrintFncPtr_t	errorPrintFncPtr;
	static resetFncPtr_t		amuResetFncPtr;
	static resetFncPtr_t		eyasResetFncPtr;

	uint8_t						address;
	volatile amu_device_t*		amu_dev;

	char						serial_number[AMU_SERIALNUM_STR_LEN];
	char						firmware[AMU_FIRMWARE_STR_LEN];

	amu_hardware_revision_t		hardware_revision;

	amu_dut_t					dut;

	ivsweep_config_t			sweep_config;
	ivsweep_meta_t				meta;

	quad_photo_sensor_t			sun_sensor;

	void						initState(void);

	// -----------------------------------------------------------------------
	// Command transport
	// -----------------------------------------------------------------------

	int8_t						busy(void);

	int8_t						sendCommand(CMD_t cmd);
	int8_t						sendCommand(CMD_t cmd, void* params, uint8_t param_len);
	int8_t						sendCommandandWait(CMD_t cmd, uint32_t wait);

	/** @brief Issue an extended command.
	 *
	 * Extended commands escape through @c CMD_SYSTEM_EXTENDED: the 16-bit value
	 * occupies bytes 0-1 of the transfer register and @p params follows from byte 2.
	 * @see CMD_EXT_t */
	int8_t						sendExtCommand(CMD_EXT_t ext);
	int8_t						sendExtCommand(CMD_EXT_t ext, const void* params, size_t param_len);

	/** @brief Issue an extended command and collect its response.
	 *  @return 0 on success, nonzero TWI error otherwise */
	int8_t						queryExtCommand(CMD_EXT_t ext, const void* params, size_t param_len, void* response, size_t response_len);

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

#endif // __AMU_CLASS__

#endif	/* __AMULIB_H__ */
