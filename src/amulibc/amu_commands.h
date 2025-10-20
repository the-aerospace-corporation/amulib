/**
 * @file amu_commands.h
 * @brief AMU Command Definitions and SCPI Command Mappings
 *
 * This file defines all command IDs used by the AMU (Aerospace Measurement Unit) for both
 * I2C and USB communication interfaces. Each command is mapped to specific SCPI strings
 * in scpi.h to provide a standardized instrument control interface.
 * 
 * @section cmd_structure Command Structure
 * Commands are organized hierarchically with offset-based grouping:
 * - System commands: Device control and status
 * - DUT commands: Device Under Test configuration  
 * - Execution commands: Measurement and calibration operations
 * - Sweep commands: I-V sweep control
 * - Auxiliary commands: DAC, heater, and sensor control
 * - ADC commands: Channel configuration and calibration
 * 
 * @section cmd_interfaces Supported Interfaces
 * - I2C: All commands with CMD_I2C_USB prefix
 * - USB: All commands including CMD_USB_ONLY prefix
 * 
 * @author	CJM28241
 * @date	10/25/2018 10:47:55 PM
 */ 


#ifndef __AMU_COMMANDS_H__
#define __AMU_COMMANDS_H__

#include "amu_types.h"
#include "amu_config_internal.h"

#define CMD_RW_BIT		7
#define CMD_READ		(1<<CMD_RW_BIT)
#define CMD_WRITE		(0<<CMD_RW_BIT)

/**
 * @brief TODO
 * 
 * Command structure is a single byte, wherein the highest bit determines
 * whether the command is a read or write operation.
 * The CMD_OFFSET is used when processing the command to indicate routing
 * callbacks or the amu internal registers.
 */

#define CMD_I2C_USB				0x0100		/*!< Root command for I2C/USB commands */
#define CMD_USB_ONLY			0x0200		/*!< Root command for USB only commands */

#define CMD_SYSTEM_OFFSET					CMD_I2C_USB + 0x00
#define CMD_SYSTEM_LED_OFFSET				CMD_I2C_USB + 0x10
#define CMD_DUT_OFFSET						CMD_I2C_USB + 0x20
#define CMD_EXEC_OFFSET						CMD_I2C_USB + 0x30
#define CMD_SWEEP_OFFSET					CMD_I2C_USB + 0x40
#define CMD_AUX_OFFSET						CMD_I2C_USB + 0x50
#define CMD_ADC_CH_OFFSET					CMD_I2C_USB + 0x60
#define CMD_MEAS_CH_CMD_OFFSET				CMD_I2C_USB + 0x70

#define CMD_USB_SYSTEM_OFFSET				CMD_USB_ONLY + 0x00
#define CMD_USB_EEPROM_CMD_OFFSET			CMD_USB_ONLY + 0x10
#define CMD_USB_ADC_VOLTAGE_CMD_OFFSET		CMD_USB_ONLY + 0x20
#define CMD_USB_ADC_CURRENT_CMD_OFFSET		CMD_USB_ONLY + 0x30
#define CMD_USB_SWEEP_OFFSET				CMD_USB_ONLY + 0x40
#define CMD_USB_SYSTEM_CONFIG_OFFSET		CMD_USB_ONLY + 0x50

typedef enum {
	CMD_SYSTEM =				CMD_SYSTEM_OFFSET,
	CMD_SYSTEM_LED =			CMD_SYSTEM_LED_OFFSET,
	CMD_DUT =					CMD_DUT_OFFSET,
	CMD_EXEC =					CMD_EXEC_OFFSET,
	CMD_SWEEP =					CMD_SWEEP_OFFSET,
	CMD_AUX =					CMD_AUX_OFFSET,
	CMD_ADC_CH =				CMD_ADC_CH_OFFSET,
	CMD_MEAS_CH =				CMD_MEAS_CH_CMD_OFFSET,
	
	CMD_USB_SYSTEM =			CMD_USB_SYSTEM_OFFSET,
	CMD_USB_EEPROM =			CMD_USB_EEPROM_CMD_OFFSET,
	CMD_USB_ADC_VOLTAGE =		CMD_USB_ADC_VOLTAGE_CMD_OFFSET,
	CMD_USB_ADC_CURRENT =		CMD_USB_ADC_CURRENT_CMD_OFFSET,
	CMD_USB_SWEEP =				CMD_USB_SWEEP_OFFSET,
	CMD_USB_SYSTEM_CONFIG =		CMD_USB_SYSTEM_CONFIG_OFFSET
} CMD_t;

#define AMU_GET_CMD_ROOT(cmd)		(cmd & 0xFF70)		/*!< clear the lower nibble and the read bit */
#define AMU_GET_CMD_BRANCH(cmd)		(cmd & 0x000F)		/*!< remove lower nibble (might not be used...) */
// With the READ WRITE BIT, we only have SEVEN available "root" enums of 16 comands each
// 


/**
 * @brief I2C System command identifiers
 * @ingroup i2c_system_commands
 * 
 * System commands control basic device functionality and provide device information.
 * These I2C commands are available on both I2C and USB interfaces.
 * Many have corresponding SCPI command equivalents.
 */
typedef enum {
	/** @brief No operation command - Communication test
	 *  @details Tests device communication without performing any operation or changing state.
	 *           Used for verifying interface connectivity and basic device responsiveness.
	 *  @param None
	 *  @return Status code (0 = success)
	 *  @par SCPI Equivalent:
	 *  `*TST?`
	 */
	CMD_SYSTEM_NO_CMD =						CMD_SYSTEM_OFFSET + 0x00,
	
	/** @brief Performs complete software reset
	 *  @details Performs a complete software reset of the device, restoring all settings 
	 *           to factory defaults and reinitializing all subsystems.
	 *  @par SCPI Equivalent:
	 *  `*RST`
	 *  @warning All user configuration will be lost
	 *  @note Device will disconnect briefly during reset process
	 */
	CMD_SYSTEM_RESET =						CMD_SYSTEM_OFFSET + 0x01,
	
	/** @brief Returns microcontroller fuse configuration
	 *  @details Returns the microcontroller fuse configuration as a 32-bit hexadecimal value.
	 *           Fuse bits control fundamental hardware behavior and security features.
	 *  @return 32-bit fuse configuration (hex format: 0xAABBCCDD)
	 *  @par SCPI Equivalent:
	 *  `SYSTem:XMEGA:FUSES?`
	 *  @note Read-only command for diagnostic purposes
	 */
	CMD_SYSTEM_XMEGA_FUSES =				CMD_SYSTEM_OFFSET + 0x02,
	
	/** @brief Sets or queries I2C slave address
	 *  @details Sets or queries the device I2C slave address for TWI communication.
	 *           Address changes take effect immediately but are not saved to EEPROM.
	 *  @param address 7-bit I2C address (range: 0x08-0x77, excludes reserved addresses)
	 *  @return Current I2C address (7-bit, no R/W bit)
	 *  @par SCPI Equivalent:
	 *  `SYSTem:TWI:ADDress[?]`
	 *  @warning Address 0x00-0x07 and 0x78-0x7F are reserved and will be rejected
	 */
	CMD_SYSTEM_TWI_ADDRESS =				CMD_SYSTEM_OFFSET + 0x03,
	
	/** @brief Scans I2C bus for devices
	 *  @details Performs an I2C bus scan and returns the number of responding devices.
	 *           Scans all valid 7-bit addresses (0x08-0x77) and counts acknowledgments.
	 *  @return Number of detected I2C devices (0-112)
	 *  @par SCPI Equivalent:
	 *  `SYSTem:TWI:NUMdevices?`
	 *  @note Scan may take up to 2 seconds to complete
	 */
	CMD_SYSTEM_TWI_NUM_DEVICES =			CMD_SYSTEM_OFFSET + 0x04,
	
	/** @brief Returns I2C interface status
	 *  @details Returns comprehensive I2C interface status including error conditions,
	 *           bus state, and communication statistics.
	 *  @return Status bitfield: [7:4]=Reserved [3]=Bus_Error [2]=Arbitration_Lost [1]=NACK [0]=Active
	 *  @par SCPI Equivalent:
	 *  `SYSTem:TWI:STATus?`
	 *  @note Status bits are cleared after reading
	 */
	CMD_SYSTEM_TWI_STATUS =					CMD_SYSTEM_OFFSET + 0x05,
	
	/** @brief Returns firmware version string
	 *  @details Returns the current firmware version string in semantic versioning format.
	 *           Includes major.minor.patch version numbers.
	 *  @return Version string (format: "X.Y.Z", e.g., "2.1.0")
	 *  @par SCPI Equivalent:
	 *  `SYSTem:FIRMware?`
	 *  @note Maximum string length: 16 characters
	 */
	CMD_SYSTEM_FIRMWARE =					CMD_SYSTEM_OFFSET + 0x06,
	
	/** @brief Returns unique device serial number
	 *  @details Returns the factory-programmed unique serial number for device identification
	 *           and traceability. Serial number is burned into EEPROM during manufacturing.
	 *  @return Serial number string (format: "AMU-YYYYMMDD-XXXX")
	 *  @par SCPI Equivalent:
	 *  `SYSTem:SERial?`
	 *  @note Serial numbers are globally unique across all devices
	 */
	CMD_SYSTEM_SERIAL_NUM =					CMD_SYSTEM_OFFSET + 0x07,
	
	/** @brief Internal MCU temperature sensor
	 *  @details Reads the built-in microcontroller temperature sensor for thermal monitoring.
	 *           Accuracy is ±3°C, primarily used for thermal protection and diagnostics.
	 *  @param None
	 *  @return Temperature in degrees Celsius (range: -40 to +85°C)
	 *  @par SCPI Equivalent:
	 *  `SYSTem:TEMPerature?`
	 *  @note Sensor is factory calibrated at 25°C
	 *  @warning High temperatures may indicate thermal stress
	 */
	CMD_SYSTEM_TEMPERATURE =				CMD_SYSTEM_OFFSET + 0x08,
	
	/** @brief Sets or queries system timestamp
	 *  @details Sets or queries the system timestamp representing seconds since device boot.
	 *           Useful for timing measurements and event correlation.
	 *  @param timestamp Seconds since boot (32-bit unsigned, rolls over at ~136 years)
	 *  @return Current timestamp in seconds
	 *  @par SCPI Equivalent:
	 *  `SYSTem:TIMEstamp[?]`
	 *  @note Timestamp is reset to 0 on power cycle or reset
	 */
	CMD_SYSTEM_TIME =						CMD_SYSTEM_OFFSET + 0x09,
	
	/** @brief Sets or queries UTC timestamp
	 *  @details Sets or queries absolute UTC timestamp in Unix epoch format (seconds since Jan 1, 1970).
	 *           Requires external time synchronization for accuracy.
	 *  @param utc_time Unix epoch timestamp (32-bit, valid until 2038)
	 *  @return Current UTC timestamp
	 *  @par SCPI Equivalent:
	 *  `SYSTem:TIMEstamp:UTC[?]`
	 *  @note Time is not maintained across power cycles without external RTC
	 */
	CMD_SYSTEM_UTC_TIME =					CMD_SYSTEM_OFFSET + 0x0A,
	
	/** @brief Sets or queries status LED color
	 *  @details Sets or queries the status LED RGB color values for visual feedback.
	 *           Each color component uses floating-point values for smooth transitions.
	 *  @param red Red component intensity (range: 0.0-1.0)
	 *  @param green Green component intensity (range: 0.0-1.0) 
	 *  @param blue Blue component intensity (range: 0.0-1.0)
	 *  @return Current RGB values as comma-separated floats
	 *  @par SCPI Equivalent:
	 *  `SYSTem:LED:COLOR[?]`
	 *  @note LED brightness is automatically adjusted for optimal visibility
	 */
	CMD_SYSTEM_LED_COLOR =					CMD_SYSTEM_OFFSET + 0x0B,
	
	/** @brief Returns microcontroller signature bytes
	 *  @details Returns the microcontroller's unique signature bytes for device identification
	 *           and debugging purposes. Contains manufacturer ID and device type information.
	 *  @return 3-byte signature (format: 0xAABBCC)
	 *  @par SCPI Equivalent:
	 *  `SYSTem:XMEGA:SIGnature?`
	 *  @note Signature is factory-programmed and cannot be modified
	 */
	CMD_SYSTEM_XMEGA_USER_SIGNATURES =		CMD_SYSTEM_OFFSET + 0x0C,
	
	/** @brief Enters low-power sleep mode
	 *  @details Puts the device into low-power sleep mode to conserve energy.
	 *           Device will wake on USB activity, I2C communication, or external interrupt.
	 *  @par SCPI Equivalent:
	 *  `SYSTem:SLEEP`
	 *  @note Current measurements will be suspended during sleep
	 *  @warning USB communication may be interrupted briefly
	 */
	CMD_SYSTEM_SLEEP =						CMD_SYSTEM_OFFSET + 0x0F,
} CMD_SYSTEM_t;
#undef CMD_SYSTEM_OFFSET

/**
 * @brief Device Under Test (DUT) command identifiers
 * @ingroup i2c_dut_commands
 * 
 * DUT commands configure and query parameters related to the device being tested.
 * These include physical characteristics, manufacturer information, and calibration data.
 */
typedef enum {
	/** @brief Sets or queries DUT junction type
	 *  @details Sets or queries the DUT junction type identifier for proper measurement
	 *           configuration. Junction type affects I-V curve characteristics and measurement parameters.
	 *  @param junction Junction type ID (0=Unknown, 1=Silicon, 2=GaAs, 3=InGaP, 4=Ge)
	 *  @return Current junction type identifier
	 *  @par SCPI Equivalent:
	 *  `DUT:JUNCtion[?]`
	 *  @note Junction type affects measurement range and calibration coefficients
	 */
	CMD_DUT_JUNCTION =						CMD_DUT_OFFSET + 0x00,
	
	/** @brief Sets or queries DUT coverglass type
	 *  @details Sets or queries the DUT coverglass type for optical correction factors.
	 *           Coverglass thickness and material affect light transmission characteristics.
	 *  @param coverglass Coverglass type ID (0=None, 1=0.1mm, 2=0.2mm, 3=0.3mm, 4=Custom)
	 *  @return Current coverglass type identifier  
	 *  @par SCPI Equivalent:
	 *  `DUT:COVERglass[?]`
	 *  @note Affects optical calibration and measurement corrections
	 */
	CMD_DUT_COVERGLASS =					CMD_DUT_OFFSET + 0x01,
	
	/** @brief Sets or queries DUT interconnect type
	 *  @details Sets or queries the DUT interconnect type for resistance compensation.
	 *           Different interconnect methods have varying parasitic resistances.
	 *  @param interconnect Interconnect type ID (0=Wire bonds, 1=Solder, 2=Conductive epoxy, 3=Other)
	 *  @return Current interconnect type identifier
	 *  @par SCPI Equivalent:
	 *  `DUT:INTERconnect[?]`
	 *  @note Affects series resistance compensation in measurements
	 */
	CMD_DUT_INTERCONNECT =					CMD_DUT_OFFSET + 0x02,
	
	/** @brief Reserved for future expansion
	 *  @details Reserved command ID for future DUT configuration features.
	 *           Currently not implemented - will return error if accessed.
	 *  @warning Do not use - reserved for future firmware versions
	 */
	CMD_DUT_RESERVED =						CMD_DUT_OFFSET + 0x03,
	
	/** @brief Sets or queries DUT manufacturer name
	 *  @details Sets or queries the DUT manufacturer name string for documentation
	 *           and traceability purposes. Stored in device memory for test reports.
	 *  @param manufacturer Manufacturer name string (max 32 characters)
	 *  @return Current manufacturer name
	 *  @par SCPI Equivalent:
	 *  `DUT:MANufacturer[?]`
	 *  @note Used in automated test report generation
	 */
	CMD_DUT_MANUFACTURER =					CMD_DUT_OFFSET + 0x04,
	
	/** @brief Sets or queries DUT model/part number
	 *  @details Sets or queries the DUT model/part number string for device identification.
	 *           Critical for maintaining test data correlation with specific device types.
	 *  @param model Model/part number string (max 32 characters)
	 *  @return Current model designation
	 *  @par SCPI Equivalent:
	 *  `DUT:MODel[?]`
	 *  @note Essential for test data traceability and analysis
	 */
	CMD_DUT_MODEL =							CMD_DUT_OFFSET + 0x05,
	
	/** @brief Sets or queries DUT semiconductor technology
	 *  @details Sets or queries the DUT semiconductor technology description for
	 *           proper measurement parameter selection and data interpretation.
	 *  @param technology Technology string (e.g., "Silicon", "GaAs", "InGaP/GaAs/Ge", max 32 chars)
	 *  @return Current technology description
	 *  @par SCPI Equivalent:
	 *  `DUT:TECHnology[?]`
	 *  @note Affects default measurement parameters and analysis algorithms
	 */
	CMD_DUT_TECHNOLOGY =					CMD_DUT_OFFSET + 0x06,
	
	/** @brief Sets or queries DUT serial number
	 *  @details Sets or queries the DUT serial number for individual device tracking
	 *           throughout testing and analysis phases.
	 *  @param serial Serial number string (max 32 characters)
	 *  @return Current DUT serial number
	 *  @par SCPI Equivalent:
	 *  `DUT:SERialnumber[?]`
	 *  @note Critical for individual device performance tracking
	 */
	CMD_DUT_SERIAL_NUMBER =					CMD_DUT_OFFSET + 0x07,
	
	/** @brief Sets or queries DUT radiation energy exposure
	 *  @details Sets or queries the total radiation energy exposure accumulated by the DUT.
	 *           Used for tracking radiation degradation effects on device performance.
	 *  @param energy Total energy exposure in MeV (floating-point, 0.0 to 1e12)
	 *  @return Current cumulative energy exposure
	 *  @par SCPI Equivalent:
	 *  `DUT:ENERGY[?]`
	 *  @note Essential for radiation effects analysis and modeling
	 */
	CMD_DUT_ENERGY =						CMD_DUT_OFFSET + 0x08,
	
	/** @brief Sets or queries DUT ionizing radiation dose
	 *  @details Sets or queries the total ionizing radiation dose accumulated by the DUT.
	 *           Tracks total ionizing dose (TID) effects on device characteristics.
	 *  @param dose Total dose in krad(Si) (floating-point, 0.0 to 10000.0)
	 *  @return Current cumulative dose exposure
	 *  @par SCPI Equivalent:
	 *  `DUT:DOSE[?]`
	 *  @note Used for TID degradation analysis and lifetime predictions
	 */
	CMD_DUT_DOSE =							CMD_DUT_OFFSET + 0x09,
	
	/** @brief Sets or queries DUT documentation notes
	 *  @details Sets or queries free-form text notes about the DUT for documentation
	 *           and special handling instructions. Supports markdown formatting.
	 *  @param notes Text notes string (max 256 characters)
	 *  @return Current notes content
	 *  @par SCPI Equivalent:
	 *  `DUT:NOTEs[?]`
	 *  @note Supports basic markdown formatting for rich documentation
	 */
	CMD_DUT_NOTES =							CMD_DUT_OFFSET + 0x0B,
	
	/** @brief Sets or queries DUT temperature sensor type
	 *  @details Sets or queries the type of temperature sensor attached to the DUT
	 *           for thermal monitoring during testing. Affects calibration algorithms.
	 *  @param type Sensor type ID (0=None, 1=Thermocouple, 2=RTD, 3=Thermistor, 4=Diode)
	 *  @return Current sensor type identifier
	 *  @par SCPI Equivalent:
	 *  `DUT:TSENSor:TYPE[?]`
	 *  @note Sensor type determines calibration coefficients and measurement range
	 */
	CMD_DUT_TSENSOR_TYPE =					CMD_DUT_OFFSET + 0x0D,
	
	/** @brief Sets or queries number of DUT temperature sensors
	 *  @details Sets or queries the number of temperature sensors connected to the DUT
	 *           for multi-point thermal monitoring and gradient measurements.
	 *  @param count Number of sensors (range: 0-4)
	 *  @return Current number of configured sensors
	 *  @par SCPI Equivalent:
	 *  `DUT:TSENSor:NUMber[?]`
	 *  @note Maximum 4 sensors supported per DUT
	 */
	CMD_DUT_TSENSOR_NUMBER =				CMD_DUT_OFFSET + 0x0E,
	
	/** @brief Sets or queries temperature sensor calibration coefficients
	 *  @details Sets or queries the temperature sensor calibration polynomial coefficients
	 *           for accurate temperature conversion from raw ADC readings.
	 *  @param coeffs Polynomial coefficients array [a0, a1, a2, a3] (T = a0 + a1*x + a2*x² + a3*x³)
	 *  @return Current calibration coefficients
	 *  @par SCPI Equivalent:
	 *  `DUT:TSENSor:FIT[?]`
	 *  @note Coefficients must be determined through calibration procedure
	 */
	CMD_DUT_TSENSOR_FIT =					CMD_DUT_OFFSET + 0x0F,
} CMD_DUT_t;
#undef CMD_DUT_OFFSET

/**
 * @brief Execution and measurement command identifiers
 * @ingroup i2c_measurement_commands
 * 
 * These commands trigger measurements and calibration procedures.
 * Most return measurement data or perform calibration operations.
 */
typedef enum {
	/** @brief Measures all active ADC channels
	 *  @details Returns measurements from all currently active ADC channels
	 *  @return Measurement data from all active channels
	 *  @par SCPI Equivalent:
	 *  `MEASure:ADC:ACTive[:RAW]?`
	 */
	CMD_EXEC_MEAS_ACTIVE_CHANNELS =			CMD_EXEC_OFFSET + 0x00,
	
	/** @brief Measures specified ADC channel
	 *  @details Returns measurement from specified ADC channel (0-15)
	 *  @param channel ADC channel number (0-15)
	 *  @return Measurement data from specified channel
	 *  @par SCPI Equivalent:
	 *  `MEASure:ADC:CH#[:RAW]?`
	 */
	CMD_EXEC_MEAS_CHANNEL =					CMD_EXEC_OFFSET + 0x01,
	
	/** @brief Measures all configured temperature sensors
	 *  @details Returns temperature measurements from all configured sensors
	 *  @return Temperature data from all configured sensors
	 *  @par SCPI Equivalent:
	 *  `MEASure:ADC:TSENSORS[:RAW]?`
	 */
	CMD_EXEC_MEAS_TSENSORS =				CMD_EXEC_OFFSET + 0x02,
	
	/** @brief Measures internal supply voltages
	 *  @details Returns internal supply voltage measurements (AVDD, IOVDD, etc.)
	 *  @return Internal supply voltage measurements
	 *  @par SCPI Equivalent:
	 *  `MEASure:INTERNALvoltages?`
	 */
	CMD_EXEC_MEAS_INTERNAL_VOLTAGES =		CMD_EXEC_OFFSET + 0x03,
	
	/** @brief Calculates sun sensor angles
	 *  @details Returns calculated sun sensor yaw and pitch angles
	 *  @return Sun sensor yaw and pitch angle measurements
	 *  @par SCPI Equivalent:
	 *  `MEASure:SUNSensor?`
	 */
	CMD_EXEC_MEAS_SUN_SENSOR =				CMD_EXEC_OFFSET + 0x04,
	
	/** @brief Measures pressure sensor
	 *  @details Returns pressure sensor measurement data
	 *  @return Pressure sensor measurement
	 *  @par SCPI Equivalent:
	 *  `MEASure:PRESSure?`
	 */
	CMD_EXEC_MEAS_PRESSURE_SENSOR =			CMD_EXEC_OFFSET + 0x05,
	
	/** @brief Initiates ADC calibration procedure
	 *  @details Initiates ADC calibration procedure and returns calibration value
	 *  @return ADC calibration value
	 *  @par SCPI Equivalent:
	 *  `ADC:CALibrate[?]`
	 */
	CMD_EXEC_ADC_CAL =						CMD_EXEC_OFFSET + 0x07,
	
	/** @brief Calibrates all internal ADC references
	 *  @details Performs calibration on all internal ADC reference channels
	 *  @par SCPI Equivalent:
	 *  `ADC:CALibrate:ALL:INTernal`
	 */
	CMD_EXEC_ADC_CAL_ALL_INTERNAL =			CMD_EXEC_OFFSET + 0x08,
	
	/** @brief Saves internal ADC calibration data
	 *  @details Saves all internal ADC calibration data to EEPROM
	 *  @par SCPI Equivalent:
	 *  `ADC:SAVE:ALL:INTernal`
	 */
	CMD_EXEC_ADC_CAL_SAVE_ALL_INTERNAL =	CMD_EXEC_OFFSET + 0x09,
	
	/** @brief Initiates DAC calibration procedure
	 *  @details Initiates DAC calibration procedure and returns calibration value
	 *  @return DAC calibration value
	 *  @par SCPI Equivalent:
	 *  `DAC:CALibrate[?]`
	 */
	CMD_EXEC_DAC_CAL =						CMD_EXEC_OFFSET + 0x0A,
	
	/** @brief Saves DAC calibration data
	 *  @details Saves DAC calibration data to EEPROM
	 *  @par SCPI Equivalent:
	 *  `DAC:CALibrate:SAVe`
	 */
	CMD_EXEC_DAC_CAL_SAVE =					CMD_EXEC_OFFSET + 0x0B,
	
	/** @brief Calibrates temperature sensor at 25°C
	 *  @details Performs temperature sensor calibration at 25°C reference
	 *  @par SCPI Equivalent:
	 *  `ADC:CALibrate:TSENSor`
	 */
	CMD_EXEC_TSENSOR_CAL_25C =				CMD_EXEC_OFFSET + 0x0C,
	
	/** @brief Saves temperature sensor calibration coefficients
	 *  @details Saves temperature sensor calibration coefficients to EEPROM
	 *  @par SCPI Equivalent:
	 *  `DUT:TSENSor:FIT:SAVE`
	 */
	CMD_EXEC_TSENSOR_COEFF_SAVE =			CMD_EXEC_OFFSET + 0x0D,
	
	/** @brief Saves sun sensor calibration coefficients
	 *  @details Saves sun sensor calibration coefficients to EEPROM
	 *  @par SCPI Equivalent:
	 *  `SUNSensor:FIT:SAVE`
	 */
	CMD_EXEC_SUNSENSOR_COEFF_SAVE =			CMD_EXEC_OFFSET + 0x0E,
	
	/** @brief Saves heater PID controller parameters
	 *  @details Saves heater PID controller parameters to EEPROM
	 *  @par SCPI Equivalent:
	 *  `HEATer:PID:SAVE`
	 */
	CMD_EXEC_HEATER_PID_SAVE  =				CMD_EXEC_OFFSET + 0x0F,
} CMD_EXEC_t;
#undef CMD_EXEC_OFFSET

/**
 * @brief I-V sweep control command identifiers
 * @ingroup i2c_sweep_commands
 * 
 * These commands control current-voltage sweep operations for solar cell characterization.
 * Sweeps can be configured, triggered, and data can be stored/retrieved from EEPROM.
 */
typedef enum {
	/** @brief Disables sweep functionality
	 *  @details Disables sweep functionality and stops any ongoing sweep
	 *  @par SCPI Equivalent:
	 *  `SWEEP:DISable`
	 */
	CMD_SWEEP_DISABLE =						CMD_SWEEP_OFFSET + 0x00,
	
	/** @brief Enables sweep functionality
	 *  @details Enables sweep functionality for I-V measurements
	 *  @par SCPI Equivalent:
	 *  `SWEEP:ENAble`
	 */
	CMD_SWEEP_ENABLE =						CMD_SWEEP_OFFSET + 0x01,
	
	/** @brief Initiates complete I-V sweep measurement
	 *  @details Initiates a complete I-V sweep measurement
	 *  @par SCPI Equivalent:
	 *  `SWEEP:TRIGger`
	 */
	CMD_SWEEP_TRIG_SWEEP =					CMD_SWEEP_OFFSET + 0x02,
	
	/** @brief Measures short-circuit current
	 *  @details Measures and returns short-circuit current value
	 *  @return Short-circuit current (Isc) value
	 *  @par SCPI Equivalent:
	 *  `SWEEP:TRIGger:ISC?`
	 */
	CMD_SWEEP_TRIG_ISC =					CMD_SWEEP_OFFSET + 0x03,
	
	/** @brief Measures open-circuit voltage
	 *  @details Measures and returns open-circuit voltage value
	 *  @return Open-circuit voltage (Voc) value
	 *  @par SCPI Equivalent:
	 *  `SWEEP:TRIGger:VOC?`
	 */
	CMD_SWEEP_TRIG_VOC =					CMD_SWEEP_OFFSET + 0x04,
	
	/** @brief Saves sweep configuration
	 *  @details Saves current sweep configuration to EEPROM
	 *  @par SCPI Equivalent:
	 *  `SWEEP:CONFig:SAVe`
	 */
	CMD_SWEEP_CONFIG_SAVE =					CMD_SWEEP_OFFSET + 0x05,
	
	/** @brief Saves sweep data to EEPROM
	 *  @details Saves sweep measurement data to EEPROM
	 *  @par SCPI Equivalent:
	 *  `SWEEP:EEPROM:SAVE`
	 */
	CMD_SWEEP_IV_SAVE_TO_EEPROM =			CMD_SWEEP_OFFSET + 0x06,
	
	/** @brief Loads sweep data from EEPROM
	 *  @details Loads previously saved sweep data from EEPROM
	 *  @return Previously saved sweep measurement data
	 *  @par SCPI Equivalent:
	 *  `SWEEP:EEPROM:LOAD`
	 */
	CMD_SWEEP_IV_LOAD_FROM_EEPROM =			CMD_SWEEP_OFFSET + 0x07,
	
	/** @brief Encrypts sweep data using AES
	 *  @details Encrypts sweep data using AES encryption
	 *  @par SCPI Equivalent:
	 *  `SWEEP:AES:ENCode`
	 */
	CMD_SWEEP_AES_ENCODE =					CMD_SWEEP_OFFSET + 0x08,
	
	/** @brief Decrypts AES-encrypted sweep data
	 *  @details Decrypts AES-encrypted sweep data
	 *  @par SCPI Equivalent:
	 *  `SWEEP:AES:DECode`
	 */
	CMD_SWEEP_AES_DECODE =					CMD_SWEEP_OFFSET + 0x09,
	
	/** @brief Saves single voltage/current data point
	 *  @details Saves a single voltage/current data point
	 *  @par SCPI Equivalent:
	 *  `SWEEP:DATApoint:SAVE`
	 */
	CMD_SWEEP_DATAPOINT_SAVE =				CMD_SWEEP_OFFSET + 0x0A,
	
	/** @brief Initializes sweep trigger system
	 *  @details Initializes the sweep trigger system
	 *  @par SCPI Equivalent:
	 *  `SWEEP:TRIGger:INITialize`
	 */
	CMD_SWEEP_TRIG_INIT =					CMD_SWEEP_OFFSET + 0x0B,
	
	/** @brief Loads single voltage/current data point
	 *  @details Loads a single voltage/current data point
	 *  @return Single voltage/current data point
	 *  @par SCPI Equivalent:
	 *  `SWEEP:DATApoint:LOAD`
	 */
	CMD_SWEEP_DATAPOINT_LOAD =				CMD_SWEEP_OFFSET + 0x0C,
} CMD_SWEEP_t;
#undef CMD_SWEEP_OFFSET

/**
 * @brief Auxiliary subsystem command identifiers
 * @ingroup i2c_aux_commands
 * 
 * These commands control auxiliary subsystems including DAC output, heater control,
 * and sun sensor calibration. Commands support both raw values and calibrated units.
 */
typedef enum {	
	/** @brief Sets or queries DAC output enable state
	 *  @details Sets or queries DAC output enable state (0=disabled, 1=enabled)
	 *  @param state DAC enable state (0=disabled, 1=enabled)
	 *  @return Current DAC enable state
	 *  @par SCPI Equivalent:
	 *  `DAC:STATE[?]`
	 */
	CMD_AUX_DAC_STATE =						CMD_AUX_OFFSET + 0x00,
	
	/** @brief Sets or queries DAC current output
	 *  @details Sets or queries DAC current output in amperes
	 *  @param current Current output in amperes
	 *  @return Current DAC current setting in amperes
	 *  @par SCPI Equivalent:
	 *  `DAC:CURRent[?]`
	 */
	CMD_AUX_DAC_CURRENT =					CMD_AUX_OFFSET + 0x01,
	
	/** @brief Sets or queries DAC current output (raw)
	 *  @details Sets or queries DAC current output as raw 16-bit value
	 *  @param raw_value Raw 16-bit DAC value
	 *  @return Current raw DAC value
	 *  @par SCPI Equivalent:
	 *  `DAC:CURRent:RAW[?]`
	 */
	CMD_AUX_DAC_CURRENT_RAW =				CMD_AUX_OFFSET + 0x02,
	
	/** @brief Sets or queries DAC voltage output
	 *  @details Sets or queries DAC voltage output in volts
	 *  @param voltage Voltage output in volts
	 *  @return Current DAC voltage setting in volts
	 *  @par SCPI Equivalent:
	 *  `DAC:VOLTage[?]`
	 */
	CMD_AUX_DAC_VOLTAGE =					CMD_AUX_OFFSET + 0x03,
	
	/** @brief Sets or queries DAC voltage output (raw)
	 *  @details Sets or queries DAC voltage output as raw 16-bit value
	 *  @param raw_value Raw 16-bit DAC value
	 *  @return Current raw DAC voltage value
	 *  @par SCPI Equivalent:
	 *  `DAC:VOLTage:RAW[?]`
	 */
	CMD_AUX_DAC_VOLTAGE_RAW =				CMD_AUX_OFFSET + 0x04,
	
	/** @brief Sets or queries DAC offset calibration
	 *  @details Sets or queries DAC offset calibration value
	 *  @param offset DAC offset calibration value
	 *  @return Current DAC offset value
	 *  @par SCPI Equivalent:
	 *  `DAC:OFFset[?]`
	 */
	CMD_AUX_DAC_OFFSET =					CMD_AUX_OFFSET + 0x05,
	
	/** @brief Sets or queries DAC offset correction coefficient
	 *  @details Sets or queries DAC offset correction coefficient
	 *  @param correction DAC offset correction coefficient
	 *  @return Current DAC offset correction coefficient
	 *  @par SCPI Equivalent:
	 *  `DAC:OFFset:CORRection[?]`
	 */
	CMD_AUX_DAC_OFFSET_CORRECTION =			CMD_AUX_OFFSET + 0x06,
	
	/** @brief Sets or queries DAC gain correction coefficient
	 *  @details Sets or queries DAC gain correction coefficient
	 *  @param correction DAC gain correction coefficient
	 *  @return Current DAC gain correction coefficient
	 *  @par SCPI Equivalent:
	 *  `DAC:GAIN:CORRection[?]`
	 */
	CMD_AUX_DAC_GAIN_CORRECTION =			CMD_AUX_OFFSET + 0x07,
	
	/** @brief Sets or queries heater enable state
	 *  @details Sets or queries heater enable state (0=off, 1=on)
	 *  @param state Heater enable state (0=off, 1=on)
	 *  @return Current heater enable state
	 *  @par SCPI Equivalent:
	 *  `HEATer:STATE[?]`
	 */
	CMD_AUX_HEATER_STATE =					CMD_AUX_OFFSET + 0x08,
	
	/** @brief Sets or queries heater temperature setpoint
	 *  @details Sets or queries heater temperature setpoint in Celsius
	 *  @param setpoint Temperature setpoint in Celsius
	 *  @return Current heater temperature setpoint
	 *  @par SCPI Equivalent:
	 *  `HEATer:SETpoint[?]`
	 */
	CMD_AUX_HEATER_SETPOINT =				CMD_AUX_OFFSET + 0x09,
	
	/** @brief Sets or queries heater PID controller coefficients
	 *  @details Sets or queries heater PID controller coefficients (Kp, Ki, Kd)
	 *  @param coeffs PID controller coefficients [Kp, Ki, Kd]
	 *  @return Current PID controller coefficients
	 *  @par SCPI Equivalent:
	 *  `HEATer:PID[?]`
	 */
	CMD_AUX_HEATER_PID =					CMD_AUX_OFFSET + 0x0A,
	
	/** @brief Sets or queries sun sensor yaw calibration coefficients
	 *  @details Sets or queries sun sensor yaw angle calibration polynomial coefficients
	 *  @param coeffs Yaw angle calibration polynomial coefficients
	 *  @return Current yaw angle calibration coefficients
	 *  @par SCPI Equivalent:
	 *  `SUNSensor:FIT:YAW[?]`
	 */
	CMD_AUX_SUNSENSOR_FIT_YAW_COEFF =		CMD_AUX_OFFSET + 0x0B,
	
	/** @brief Sets or queries sun sensor pitch calibration coefficients
	 *  @details Sets or queries sun sensor pitch angle calibration polynomial coefficients
	 *  @param coeffs Pitch angle calibration polynomial coefficients
	 *  @return Current pitch angle calibration coefficients
	 *  @par SCPI Equivalent:
	 *  `SUNSensor:FIT:PITCH[?]`
	 */
	CMD_AUX_SUNSENSOR_FIT_PITCH_COEFF =		CMD_AUX_OFFSET + 0x0C,
	
	/** @brief Sets or queries sun sensor horizontal reference
	 *  @details Sets or queries sun sensor horizontal reference calibration value
	 *  @param hval Horizontal reference calibration value
	 *  @return Current horizontal reference value
	 *  @par SCPI Equivalent:
	 *  `SUNSensor:HVAL[?]`
	 */
	CMD_AUX_SUNSENSOR_HVAL =				CMD_AUX_OFFSET + 0x0D,
	
	/** @brief Sets or queries sun sensor radial reference
	 *  @details Sets or queries sun sensor radial reference calibration value
	 *  @param rval Radial reference calibration value
	 *  @return Current radial reference value
	 *  @par SCPI Equivalent:
	 *  `SUNSensor:RVAL[?]`
	 */
	CMD_AUX_SUNSENSOR_RVAL =				CMD_AUX_OFFSET + 0x0E,
	
	/** @brief Sets or queries sun sensor detection threshold
	 *  @details Sets or queries sun sensor detection threshold value
	 *  @param threshold Sun sensor detection threshold value
	 *  @return Current detection threshold value
	 *  @par SCPI Equivalent:
	 *  `SUNSensor:THRESHold[?]`
	 */
	CMD_AUX_SUNSENSOR_THRESHOLD =			CMD_AUX_OFFSET + 0x0F,	
} CMD_AUX_t;
#undef CMD_AUX_OFFSET

/**
 * @brief ADC channel configuration command identifiers
 * @ingroup i2c_adc_commands
 * 
 * These commands configure individual ADC channels including setup, filtering,
 * gain settings, and calibration procedures. Channel numbers range from 0-15.
 */
typedef enum {
	/** @brief Sets or queries ADC channel register
	 *  @details Sets or queries ADC channel register configuration
	 *  @param channel Channel number (0-15)
	 *  @param value Register configuration value
	 *  @return Current ADC channel register value
	 *  @par SCPI Equivalent:
	 *  `ADC:CH#[?]`
	 */
	CMD_ADC_CH_REG =						CMD_ADC_CH_OFFSET + 0x00,
	
	/** @brief Sets or queries ADC channel setup
	 *  @details Sets or queries ADC channel setup register (input selection, etc.)
	 *  @param channel Channel number (0-15)
	 *  @param setup Setup register configuration
	 *  @return Current ADC channel setup configuration
	 *  @par SCPI Equivalent:
	 *  `ADC:CH#:SETup[?]`
	 */
	CMD_ADC_CH_SETUP =						CMD_ADC_CH_OFFSET + 0x01,
	
	/** @brief Sets or queries ADC channel filter
	 *  @details Sets or queries ADC channel digital filter settings
	 *  @param channel Channel number (0-15)
	 *  @param filter Digital filter configuration
	 *  @return Current ADC channel filter settings
	 *  @par SCPI Equivalent:
	 *  `ADC:CH#:FILTer[?]`
	 */
	CMD_ADC_CH_FILTER =						CMD_ADC_CH_OFFSET + 0x02,
	
	/** @brief Sets or queries ADC channel sample rate
	 *  @details Sets or queries ADC channel sample rate in Hz
	 *  @param channel Channel number (0-15)
	 *  @param rate Sample rate in Hz
	 *  @return Current ADC channel sample rate
	 *  @par SCPI Equivalent:
	 *  `ADC:CH#:RATE[?]`
	 */
	CMD_ADC_CH_RATE =						CMD_ADC_CH_OFFSET + 0x03,
	
	/** @brief Sets or queries ADC channel PGA setting
	 *  @details Sets or queries programmable gain amplifier setting (1, 2, 4, 8, 16, 32, 64, 128)
	 *  @param channel Channel number (0-15)
	 *  @param gain PGA gain setting (1, 2, 4, 8, 16, 32, 64, 128)
	 *  @return Current ADC channel PGA setting
	 *  @par SCPI Equivalent:
	 *  `ADC:CH#:PGA[?]`
	 */
	CMD_ADC_CH_PGA =						CMD_ADC_CH_OFFSET + 0x04,
	
	/** @brief Queries maximum input range for current PGA
	 *  @details Queries maximum input range for current PGA setting
	 *  @param channel Channel number (0-15)
	 *  @return Maximum input range for current PGA setting
	 *  @par SCPI Equivalent:
	 *  `ADC:CH#:MAX?`
	 */
	CMD_ADC_CH_PGA_MAX =					CMD_ADC_CH_OFFSET + 0x05,
	
	/** @brief Queries maximum voltage range for PGA
	 *  @details Queries maximum voltage range for specified PGA setting
	 *  @param pga PGA gain setting
	 *  @return Maximum voltage range for specified PGA setting
	 *  @par SCPI Equivalent:
	 *  `ADC:VOLTage:MAX:PGA#?`
	 */
	CMD_ADC_CH_PGA_VMAX =					CMD_ADC_CH_OFFSET + 0x06,
	
	/** @brief Queries maximum current range for PGA
	 *  @details Queries maximum current range for specified PGA setting
	 *  @param pga PGA gain setting
	 *  @return Maximum current range for specified PGA setting
	 *  @par SCPI Equivalent:
	 *  `ADC:CURRent:MAX:PGA#?`
	 */
	CMD_ADC_CH_PGA_IMAX =					CMD_ADC_CH_OFFSET + 0x07,
	
	/** @brief Saves ADC channel configuration
	 *  @details Saves current channel configuration to EEPROM
	 *  @param channel Channel number (0-15)
	 *  @par SCPI Equivalent:
	 *  `ADC:CH#:SAVE`
	 */
	CMD_ADC_CH_SAVE =						CMD_ADC_CH_OFFSET + 0x08,
	
	/** @brief Sets or queries ADC channel offset coefficient
	 *  @details Sets or queries ADC channel offset calibration coefficient
	 *  @param channel Channel number (0-15)
	 *  @param coefficient Offset calibration coefficient
	 *  @return Current offset calibration coefficient (int32_t format)
	 *  @par SCPI Equivalent:
	 *  `ADC:CH#:OFFset[?]`
	 */
	CMD_ADC_CH_OFFSET_COEFF =				CMD_ADC_CH_OFFSET + 0x09,
	
	/** @brief Sets or queries ADC channel gain coefficient
	 *  @details Sets or queries ADC channel gain calibration coefficient
	 *  @param channel Channel number (0-15)
	 *  @param coefficient Gain calibration coefficient
	 *  @return Current gain calibration coefficient (uint32_t format)
	 *  @par SCPI Equivalent:
	 *  `ADC:CH#:GAIN[?]`
	 */
	CMD_ADC_CH_GAIN_COEFF =					CMD_ADC_CH_OFFSET + 0x0A,
	
	/** @brief Performs internal ADC channel calibration
	 *  @details Performs internal calibration for the specified channel
	 *  @param channel Channel number (0-15)
	 *  @par SCPI Equivalent:
	 *  `ADC:CH#:CALibrate:INTernal`
	 */
	CMD_ADC_CH_CAL_INTERNAL =				CMD_ADC_CH_OFFSET + 0x0B,
	
	/** @brief Performs zero-scale ADC calibration
	 *  @details Performs zero-scale calibration for the specified channel
	 *  @param channel Channel number (0-15)
	 *  @par SCPI Equivalent:
	 *  `ADC:CH#:CALibrate:ZERO`
	 */
	CMD_ADC_CH_CAL_ZERO_SCALE =				CMD_ADC_CH_OFFSET + 0x0C,
	
	/** @brief Performs full-scale ADC calibration
	 *  @details Performs full-scale calibration for the specified channel
	 *  @param channel Channel number (0-15)
	 *  @par SCPI Equivalent:
	 *  `ADC:CH#:CALibrate:FULL`
	 */
	CMD_ADC_CH_CAL_FULL_SCALE =				CMD_ADC_CH_OFFSET + 0x0D,
	
	/** @brief Resets ADC channel calibration
	 *  @details Resets channel calibration to factory defaults
	 *  @param channel Channel number (0-15)
	 *  @par SCPI Equivalent:
	 *  `ADC:CH#:CALibrate:RESet`
	 */
	CMD_ADC_CH_CAL_RESET =					CMD_ADC_CH_OFFSET + 0x0E,
	
	/** @brief Saves ADC channel calibration
	 *  @details Saves channel calibration coefficients to EEPROM
	 *  @param channel Channel number (0-15)
	 *  @par SCPI Equivalent:
	 *  `ADC:CH#:CALibrate:SAVe`
	 */
	CMD_ADC_CH_CAL_SAVE =					CMD_ADC_CH_OFFSET + 0x0F,
} CMD_ADC_CH_t;
#undef CMD_ADC_CH_OFFSET

/**
 * @brief Measurement channel command identifiers
 * @ingroup i2c_measurement_commands
 * 
 * These commands perform measurements on specific ADC channels.
 * Channel assignments are based on AMU_ADC_CH_t definitions.
 */
typedef enum {
	/** @brief Measures dedicated voltage channel
	 *  @details Measures voltage on the dedicated voltage input channel
	 *  @return Voltage measurement from dedicated voltage channel
	 *  @par SCPI Equivalent:
	 *  `MEASure:ADC:VOLTage[:RAW]?`
	 */
	CMD_MEAS_CH_VOLTAGE =					CMD_MEAS_CH_CMD_OFFSET + 0x00,
	
	/** @brief Measures dedicated current channel
	 *  @details Measures current on the dedicated current input channel
	 *  @return Current measurement from dedicated current channel
	 *  @par SCPI Equivalent:
	 *  `MEASure:ADC:CURRent[:RAW]?`
	 */
	CMD_MEAS_CH_CURRENT =					CMD_MEAS_CH_CMD_OFFSET + 0x01,
	
	/** @brief Measures primary temperature sensor
	 *  @details Measures primary temperature sensor (same as TSENSOR0)
	 *  @return Temperature measurement from primary sensor
	 *  @par SCPI Equivalent:
	 *  `MEASure:ADC:TSENSor[:RAW]?`
	 */
	CMD_MEAS_CH_TSENSOR =					CMD_MEAS_CH_CMD_OFFSET + 0x02,
	
	/** @brief Measures temperature sensor 0
	 *  @details Measures temperature sensor 0
	 *  @return Temperature measurement from sensor 0
	 *  @par SCPI Equivalent:
	 *  `MEASure:ADC:TSENSOR0[:RAW]?`
	 */
	CMD_MEAS_CH_TSENSOR_0 =					CMD_MEAS_CH_CMD_OFFSET + 0x02,
	
	/** @brief Measures temperature sensor 1
	 *  @details Measures temperature sensor 1
	 *  @return Temperature measurement from sensor 1
	 *  @par SCPI Equivalent:
	 *  `MEASure:ADC:TSENSOR1[:RAW]?`
	 */
	CMD_MEAS_CH_TSENSOR_1 =					CMD_MEAS_CH_CMD_OFFSET + 0x03,
	
	/** @brief Measures temperature sensor 2
	 *  @details Measures temperature sensor 2
	 *  @return Temperature measurement from sensor 2
	 *  @par SCPI Equivalent:
	 *  `MEASure:ADC:TSENSOR2[:RAW]?`
	 */
	CMD_MEAS_CH_TSENSOR_2 =					CMD_MEAS_CH_CMD_OFFSET + 0x04,
	
	/** @brief Measures bias voltage reference
	 *  @details Measures bias voltage reference
	 *  @return Bias voltage reference measurement
	 *  @par SCPI Equivalent:
	 *  `MEASure:ADC:BIAS[:RAW]?`
	 */
	CMD_MEAS_CH_BIAS =						CMD_MEAS_CH_CMD_OFFSET + 0x05,
	
	/** @brief Measures offset calibration reference
	 *  @details Measures offset calibration reference
	 *  @return Offset calibration reference measurement
	 *  @par SCPI Equivalent:
	 *  `MEASure:ADC:OFFset[:RAW]?`
	 */
	CMD_MEAS_CH_OFFSET =					CMD_MEAS_CH_CMD_OFFSET + 0x06,
	
	/** @brief Measures internal MCU temperature
	 *  @details Measures internal MCU temperature sensor
	 *  @return Internal MCU temperature measurement
	 *  @par SCPI Equivalent:
	 *  `MEASure:ADC:TEMP[:RAW]?`
	 */
	CMD_MEAS_CH_TEMP =						CMD_MEAS_CH_CMD_OFFSET + 0x07,
	
	/** @brief Measures analog supply voltage
	 *  @details Measures analog supply voltage (AVDD)
	 *  @return Analog supply voltage measurement
	 *  @par SCPI Equivalent:
	 *  `MEASure:ADC:AVDD[:RAW]?`
	 */
	CMD_MEAS_CH_AVDD =						CMD_MEAS_CH_CMD_OFFSET + 0x08,
	
	/** @brief Measures I/O supply voltage
	 *  @details Measures I/O supply voltage (IOVDD)
	 *  @return I/O supply voltage measurement
	 *  @par SCPI Equivalent:
	 *  `MEASure:ADC:IOVDD[:RAW]?`
	 */
	CMD_MEAS_CH_IOVDD =						CMD_MEAS_CH_CMD_OFFSET + 0x09,
	
	/** @brief Measures analog LDO output voltage
	 *  @details Measures analog LDO regulator output voltage
	 *  @return Analog LDO regulator voltage measurement
	 *  @par SCPI Equivalent:
	 *  `MEASure:ADC:ALDO[:RAW]?`
	 */
	CMD_MEAS_CH_ALDO =						CMD_MEAS_CH_CMD_OFFSET + 0x0A,
	
	/** @brief Measures digital LDO output voltage
	 *  @details Measures digital LDO regulator output voltage
	 *  @return Digital LDO regulator voltage measurement
	 *  @par SCPI Equivalent:
	 *  `MEASure:ADC:DLDO[:RAW]?`
	 */
	CMD_MEAS_CH_DLDO =						CMD_MEAS_CH_CMD_OFFSET + 0x0B,
	
	/** @brief Measures sun sensor top-left quadrant
	 *  @details Measures sun sensor top-left quadrant photodiode
	 *  @return Sun sensor top-left quadrant measurement
	 *  @par SCPI Equivalent:
	 *  `MEASure:ADC:SSTL[:RAW]?`
	 */
	CMD_MEAS_CH_SS_TL =						CMD_MEAS_CH_CMD_OFFSET + 0x0C,
	
	/** @brief Measures sun sensor bottom-left quadrant
	 *  @details Measures sun sensor bottom-left quadrant photodiode
	 *  @return Sun sensor bottom-left quadrant measurement
	 *  @par SCPI Equivalent:
	 *  `MEASure:ADC:SSBL[:RAW]?`
	 */
	CMD_MEAS_CH_SS_BL=						CMD_MEAS_CH_CMD_OFFSET + 0x0D,
	
	/** @brief Measures sun sensor bottom-right quadrant
	 *  @details Measures sun sensor bottom-right quadrant photodiode
	 *  @return Sun sensor bottom-right quadrant measurement
	 *  @par SCPI Equivalent:
	 *  `MEASure:ADC:SSBR[:RAW]?`
	 */
	CMD_MEAS_CH_SS_BR =						CMD_MEAS_CH_CMD_OFFSET + 0x0E,
	
	/** @brief Measures sun sensor top-right quadrant
	 *  @details Measures sun sensor top-right quadrant photodiode
	 *  @return Sun sensor top-right quadrant measurement
	 *  @par SCPI Equivalent:
	 *  `MEASure:ADC:SSTR[:RAW]?`
	 */
	CMD_MEAS_CH_SS_TR =						CMD_MEAS_CH_CMD_OFFSET + 0x0F,
} CMD_MEAS_CH_t;
#undef CMD_MEAS_CH_CMD_OFFSET


/**
 * @brief USB-only system command identifiers
 * 
 * These commands are only available via USB interface and provide
 * extended system functionality not accessible over I2C.
 */
typedef enum {
	/** @brief Enter bootloader mode
	 *  @scpi_cmd SYSTem:BOOTloader
	 *  @description Enters bootloader mode for firmware updates (USB only)
	 */
	CMD_USB_SYSTEM_ENTER_BOOTLOADER =		CMD_USB_SYSTEM_OFFSET + 0x00,
	
	/** @brief List available SCPI commands
	 *  @scpi_cmd SYSTem:SCPI:LIST?
	 *  @description Returns list of all supported SCPI commands (USB only)
	 */
	CMD_USB_SYSTEM_LIST_SCPI_COMMANDS =		CMD_USB_SYSTEM_OFFSET + 0x01,
	
	/** @brief Scan I2C bus for devices
	 *  @scpi_cmd SYSTem:TWI:SCAN?
	 *  @description Scans I2C bus and returns list of device addresses (USB only)
	 */
	CMD_USB_SYSTEM_TWI_SCAN =				CMD_USB_SYSTEM_OFFSET + 0x02,
	
	/** @brief I2C mode configuration
	 *  @scpi_cmd SYSTem:TWI:MODE[?]
	 *  @description Sets or queries I2C operating mode (USB only)
	 */
	CMD_USB_SYSTEM_TWI_MODE =				CMD_USB_SYSTEM_OFFSET + 0x03,
	
	/** @brief Debug value access
	 *  @scpi_cmd SYSTem:DEBug#?
	 *  @description Sets or queries debug values for development (USB only)
	 */
	CMD_USB_SYSTEM_DEBUG =					CMD_USB_SYSTEM_OFFSET + 0x04
} CMD_USB_SYSTEM_t;
#undef CMD_USB_SYSTEM_OFFSET


/**
 * @brief USB-only EEPROM command identifiers
 * 
 * These commands provide direct EEPROM access for calibration data management.
 * Only available via USB interface for security and data integrity.
 */
typedef enum {
	/** @brief Erase all EEPROM data
	 *  @scpi_cmd MEMory:ERASE:ALL
	 *  @description Erases all user data from EEPROM (USB only)
	 */
	CMD_USB_EEPROM_ERASE_ALL =				CMD_USB_EEPROM_CMD_OFFSET + 0x00,
	
	/** @brief Erase configuration data
	 *  @scpi_cmd MEMory:ERASE:CONFig
	 *  @description Erases configuration data from EEPROM (USB only)
	 */
	CMD_USB_EEPROM_ERASE_CONFIG =			CMD_USB_EEPROM_CMD_OFFSET + 0x01,
	
	/** @brief ADC channel offset calibration data
	 *  @scpi_cmd MEMory:ADC:CH#:OFFset[?]
	 *  @description Sets or queries EEPROM-stored ADC channel offset calibration (USB only)
	 */
	CMD_USB_EEPROM_OFFSET =					CMD_USB_EEPROM_CMD_OFFSET + 0x02,
	
	/** @brief ADC channel gain calibration data
	 *  @scpi_cmd MEMory:ADC:CH#:GAIN[?]
	 *  @description Sets or queries EEPROM-stored ADC channel gain calibration (USB only)
	 */
	CMD_USB_EEPROM_GAIN =					CMD_USB_EEPROM_CMD_OFFSET + 0x03,
	
	/** @brief Voltage measurement offset calibration
	 *  @scpi_cmd MEMory:VOLTage:OFFset#[?]
	 *  @description Sets or queries EEPROM-stored voltage offset calibration (USB only)
	 */
	CMD_USB_EEPROM_VOLTAGE_OFFSET =			CMD_USB_EEPROM_CMD_OFFSET + 0x04,
	
	/** @brief Current measurement offset calibration
	 *  @scpi_cmd MEMory:CURRent:OFFset#[?]
	 *  @description Sets or queries EEPROM-stored current offset calibration (USB only)
	 */
	CMD_USB_EEPROM_CURRENT_OFFSET =			CMD_USB_EEPROM_CMD_OFFSET + 0x05,
	
	/** @brief Voltage measurement gain calibration
	 *  @scpi_cmd MEMory:VOLTage:GAIN#[?]
	 *  @description Sets or queries EEPROM-stored voltage gain calibration (USB only)
	 */
	CMD_USB_EEPROM_VOLTAGE_GAIN =			CMD_USB_EEPROM_CMD_OFFSET + 0x06,
	
	/** @brief Current measurement gain calibration
	 *  @scpi_cmd MEMory:CURRent:GAIN#[?]
	 *  @description Sets or queries EEPROM-stored current gain calibration (USB only)
	 */
	CMD_USB_EEPROM_CURRENT_GAIN =			CMD_USB_EEPROM_CMD_OFFSET + 0x07,
} CMD_USB_EEPROM_t;
#undef CMD_USB_EEPROM_CMD_OFFSET

/**
 * @brief USB-only voltage ADC command identifiers
 * 
 * These commands provide direct voltage channel control and calibration.
 * Extended functionality only available via USB interface.
 */
typedef enum {
	/** @brief Voltage channel zero-scale calibration
	 *  @scpi_cmd ADC:VOLTage:CALibrate:ZERO
	 *  @description Performs zero-scale calibration for voltage channel (USB only)
	 */
	CMD_USB_ADC_VOLTAGE_CAL_ZERO =		CMD_USB_ADC_VOLTAGE_CMD_OFFSET + 0x00,
	
	/** @brief Voltage channel full-scale calibration
	 *  @scpi_cmd ADC:VOLTage:CALibrate:FULL
	 *  @description Performs full-scale calibration for voltage channel (USB only)
	 */
	CMD_USB_ADC_VOLTAGE_CAL_FULL =		CMD_USB_ADC_VOLTAGE_CMD_OFFSET + 0x01,
	
	/** @brief Reset voltage channel calibration
	 *  @scpi_cmd ADC:VOLTage:CALibrate:RESet
	 *  @description Resets voltage channel calibration to defaults (USB only)
	 */
	CMD_USB_ADC_VOLTAGE_CAL_RESET =		CMD_USB_ADC_VOLTAGE_CMD_OFFSET + 0x02,
	
	/** @brief Save voltage channel calibration
	 *  @scpi_cmd ADC:VOLTage:CALibrate:SAVe
	 *  @description Saves voltage channel calibration to EEPROM (USB only)
	 */
	CMD_USB_ADC_VOLTAGE_CAL_SAVE =		CMD_USB_ADC_VOLTAGE_CMD_OFFSET + 0x03,
	
	/** @brief Voltage channel PGA setting
	 *  @scpi_cmd ADC:VOLTage:PGA[?]
	 *  @description Sets or queries voltage channel PGA setting (USB only)
	 */
	CMD_USB_ADC_VOLTAGE_PGA =			CMD_USB_ADC_VOLTAGE_CMD_OFFSET + 0x04,
	
	/** @brief Voltage channel maximum range
	 *  @scpi_cmd ADC:VOLTage:MAX?
	 *  @description Queries voltage channel maximum input range (USB only)
	 */
	CMD_USB_ADC_VOLTAGE_MAX =			CMD_USB_ADC_VOLTAGE_CMD_OFFSET + 0x05,
	
	/** @brief Voltage channel offset coefficient
	 *  @scpi_cmd ADC:VOLTage:OFFset[?]
	 *  @description Sets or queries voltage channel offset coefficient (USB only)
	 *  @return Voltage offset calibration coefficient (int32_t format)
	 */
	CMD_USB_ADC_VOLTAGE_OFFSET =		CMD_USB_ADC_VOLTAGE_CMD_OFFSET + 0x06,
	
	/** @brief Voltage channel gain coefficient
	 *  @scpi_cmd ADC:VOLTage:GAIN[?]
	 *  @description Sets or queries voltage channel gain coefficient (USB only)
	 *  @return Voltage gain calibration coefficient (uint32_t format)
	 */
	CMD_USB_ADC_VOLTAGE_GAIN =			CMD_USB_ADC_VOLTAGE_CMD_OFFSET + 0x07,

	/** @brief Voltage channel PGA maximum setting for given PGA setting
	 *  @scpi_cmd ADC:VOLTage:MAX:PGA[#]?
	 *  @description Queries voltage channel PGA maximum setting for given PGA setting (USB only)
	 */
	CMD_USB_ADC_VOLTAGE_MAX_PGA = 		CMD_USB_ADC_VOLTAGE_CMD_OFFSET + 0x08,

	/** @brief Voltage channel save pga settings
	 *  @scpi_cmd ADC:VOLTage:PGA:SAVE
	 *  @description Saves voltage channel PGA settings to EEPROM (USB only)
	 */
	CMD_USB_ADC_VOLTAGE_PGA_SAVE =		CMD_USB_ADC_VOLTAGE_CMD_OFFSET + 0x09,
} CMD_USB_ADC_VOLTAGE_t;
#undef CMD_USB_ADC_VOLTAGE_CMD_OFFSET


/**
 * @brief USB-only current ADC command identifiers
 * 
 * These commands provide direct current channel control and calibration.
 * Extended functionality only available via USB interface.
 */
typedef enum {
	/** @brief Current channel zero-scale calibration
	 *  @scpi_cmd ADC:CURRent:CALibrate:ZERO
	 *  @description Performs zero-scale calibration for current channel (USB only)
	 */
	CMD_USB_ADC_CURRENT_CAL_ZERO =		CMD_USB_ADC_CURRENT_CMD_OFFSET + 0x00,
	
	/** @brief Current channel full-scale calibration
	 *  @scpi_cmd ADC:CURRent:CALibrate:FULL
	 *  @description Performs full-scale calibration for current channel (USB only)
	 */
	CMD_USB_ADC_CURRENT_CAL_FULL =		CMD_USB_ADC_CURRENT_CMD_OFFSET + 0x01,
	
	/** @brief Reset current channel calibration
	 *  @scpi_cmd ADC:CURRent:CALibrate:RESet
	 *  @description Resets current channel calibration to defaults (USB only)
	 */
	CMD_USB_ADC_CURRENT_CAL_RESET =		CMD_USB_ADC_CURRENT_CMD_OFFSET + 0x02,
	
	/** @brief Save current channel calibration
	 *  @scpi_cmd ADC:CURRent:CALibrate:SAVe
	 *  @description Saves current channel calibration to EEPROM (USB only)
	 */
	CMD_USB_ADC_CURRENT_CAL_SAVE =		CMD_USB_ADC_CURRENT_CMD_OFFSET + 0x03,
	
	/** @brief Current channel PGA setting
	 *  @scpi_cmd ADC:CURRent:PGA[?]
	 *  @description Sets or queries current channel PGA setting (USB only)
	 */
	CMD_USB_ADC_CURRENT_PGA =			CMD_USB_ADC_CURRENT_CMD_OFFSET + 0x04,
	
	/** @brief Current channel maximum range
	 *  @scpi_cmd ADC:CURRent:MAX?
	 *  @description Queries current channel maximum input range (USB only)
	 */
	CMD_USB_ADC_CURRENT_MAX =			CMD_USB_ADC_CURRENT_CMD_OFFSET + 0x05,
	
	/** @brief Current channel offset coefficient
	 *  @scpi_cmd ADC:CURRent:OFFset[?]
	 *  @description Sets or queries current channel offset coefficient (USB only)
	 *  @return Current offset calibration coefficient (int32_t format)
	 */
	CMD_USB_ADC_CURRENT_OFFSET =		CMD_USB_ADC_CURRENT_CMD_OFFSET + 0x06,
	
	/** @brief Current channel gain coefficient
	 *  @scpi_cmd ADC:CURRent:GAIN[?]
	 *  @description Sets or queries current channel gain coefficient (USB only)
	 *  @return Current gain calibration coefficient (uint32_t format)
	 */
	CMD_USB_ADC_CURRENT_GAIN =			CMD_USB_ADC_CURRENT_CMD_OFFSET + 0x07,

	/** @brief Current channel PGA maximum setting for given PGA setting
	 *  @scpi_cmd ADC:CURRent:MAX:PGA[#]?
	 *  @description Queries current channel PGA maximum setting for given PGA setting (USB only)
	 */
	CMD_USB_ADC_CURRENT_MAX_PGA = 		CMD_USB_ADC_CURRENT_CMD_OFFSET + 0x08,

	/** @brief Current channel save pga settings
	 *  @scpi_cmd ADC:CURRent:PGA:SAVE
	 *  @description Saves current channel PGA settings to EEPROM (USB only)
	 */
	CMD_USB_ADC_CURRENT_PGA_SAVE =		CMD_USB_ADC_CURRENT_CMD_OFFSET + 0x09,
} CMD_USB_ADC_CURRENT_t;
#undef CMD_USB_ADC_CURRENT_CMD_OFFSET

/**
 * @brief USB-only sweep command identifiers
 * 
 * These commands provide USB-specific sweep functionality.
 */
typedef enum {
	/** @brief Trigger sweep via USB
	 *  @scpi_cmd SWEEP:TRIGger:USB
	 *  @description Triggers I-V sweep with USB-specific parameters (USB only)
	 */
	CMD_USB_SWEEP_TRIGGER =					CMD_USB_SWEEP_OFFSET + 0x00,
} CMD_USB_SWEEP_t;
#undef CMD_USB_SWEEP_OFFSET

/**
 * @brief USB-only system configuration command identifiers
 * 
 * These commands configure system-level hardware parameters and save configuration.
 * Only available via USB for security and to prevent accidental misconfiguration.
 */
typedef enum {
	/** @brief Current measurement gain setting
	 *  @scpi_cmd SYSTem:CONFig:CURRent:GAIN[?]
	 *  @description Sets or queries current measurement amplifier gain (USB only)
	 */
	CMD_USB_SYSTEM_CONFIG_CURR_GAIN =		CMD_USB_SYSTEM_CONFIG_OFFSET + 0x00,
	
	/** @brief Current measurement sense resistor value
	 *  @scpi_cmd SYSTem:CONFig:CURRent:Rsense[?]
	 *  @description Sets or queries current sense resistor value in ohms (USB only)
	 */
	CMD_USB_SYSTEM_CONFIG_CURR_RSENSE =		CMD_USB_SYSTEM_CONFIG_OFFSET + 0x01,
	
	/** @brief Voltage divider R1 resistor value
	 *  @scpi_cmd SYSTem:CONFig:VOLTage:R1[?]
	 *  @description Sets or queries voltage divider R1 resistor value in ohms (USB only)
	 */
	CMD_USB_SYSTEM_CONFIG_VOLT_R1 =			CMD_USB_SYSTEM_CONFIG_OFFSET + 0x02,
	
	/** @brief Voltage divider R2 resistor value
	 *  @scpi_cmd SYSTem:CONFig:VOLTage:R2[?]
	 *  @description Sets or queries voltage divider R2 resistor value in ohms (USB only)
	 */
	CMD_USB_SYSTEM_CONFIG_VOLT_R2 =			CMD_USB_SYSTEM_CONFIG_OFFSET + 0x03,
	
	/** @brief Save system configuration
	 *  @scpi_cmd SYSTem:CONFig:SAVE
	 *  @description Saves current system configuration to EEPROM (USB only)
	 */
	CMD_USB_SYSTEM_CONFIG_SAVE = 			CMD_USB_SYSTEM_CONFIG_OFFSET + 0x07,
} CMD_USB_SYSTEM_CONFIG_t;
#undef CMD_USB_SYSTEM_CONFIG_OFFSET




#endif /* __AMU_COMMANDS_H__ */