/**
 * @file amu_commands.h
 * @brief AMU Command Definitions and SCPI Command Mappings
 * @ingroup scpi_commands
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
 * @brief System command identifiers
 * @ingroup scpi_system_commands
 * 
 * System commands control basic device functionality and provide device information.
 * These commands are available on both I2C and USB interfaces.
 */
typedef enum {
	/** @brief No operation command - used for testing communication
	 *  @scpi_cmd *TST?
	 *  @description Tests device communication without performing any operation
	 */
	CMD_SYSTEM_NO_CMD =						CMD_SYSTEM_OFFSET + 0x00,
	
	/** @brief Software reset command
	 *  @scpi_cmd *RST  
	 *  @description Resets the device to default state
	 */
	CMD_SYSTEM_RESET =						CMD_SYSTEM_OFFSET + 0x01,
	
	/** @brief Read XMEGA fuse bits
	 *  @scpi_cmd SYSTem:XMEGA:FUSES?
	 *  @description Returns the microcontroller fuse configuration as 32-bit value
	 */
	CMD_SYSTEM_XMEGA_FUSES =				CMD_SYSTEM_OFFSET + 0x02,
	
	/** @brief I2C address configuration
	 *  @scpi_cmd SYSTem:TWI:ADDress[?]
	 *  @description Sets or queries the device I2C address (7-bit, range 0x08-0x77)
	 */
	CMD_SYSTEM_TWI_ADDRESS =				CMD_SYSTEM_OFFSET + 0x03,
	
	/** @brief Number of I2C devices on bus
	 *  @scpi_cmd SYSTem:TWI:NUMdevices?
	 *  @description Returns the number of detected I2C devices on the bus
	 */
	CMD_SYSTEM_TWI_NUM_DEVICES =			CMD_SYSTEM_OFFSET + 0x04,
	
	/** @brief I2C communication status
	 *  @scpi_cmd SYSTem:TWI:STATus?
	 *  @description Returns I2C interface status and error flags
	 */
	CMD_SYSTEM_TWI_STATUS =					CMD_SYSTEM_OFFSET + 0x05,
	
	/** @brief Firmware version string
	 *  @scpi_cmd SYSTem:FIRMware?
	 *  @description Returns firmware version string (e.g., "2.1.0")
	 */
	CMD_SYSTEM_FIRMWARE =					CMD_SYSTEM_OFFSET + 0x06,
	
	/** @brief Device serial number
	 *  @scpi_cmd SYSTem:SERial?
	 *  @description Returns unique device serial number string
	 */
	CMD_SYSTEM_SERIAL_NUM =					CMD_SYSTEM_OFFSET + 0x07,
	
	/** @brief Internal temperature measurement
	 *  @scpi_cmd SYSTem:TEMPerature?
	 *  @description Returns internal MCU temperature in degrees Celsius
	 */
	CMD_SYSTEM_TEMPERATURE =				CMD_SYSTEM_OFFSET + 0x08,
	
	/** @brief System timestamp
	 *  @scpi_cmd SYSTem:TIMEstamp[?]
	 *  @description Sets or queries system timestamp (seconds since boot)
	 */
	CMD_SYSTEM_TIME =						CMD_SYSTEM_OFFSET + 0x09,
	
	/** @brief UTC timestamp
	 *  @scpi_cmd SYSTem:TIMEstamp:UTC[?]
	 *  @description Sets or queries UTC timestamp (Unix epoch seconds)
	 */
	CMD_SYSTEM_UTC_TIME =					CMD_SYSTEM_OFFSET + 0x0A,
	
	/** @brief LED color control
	 *  @scpi_cmd SYSTem:LED:COLOR[?]
	 *  @description Sets or queries LED RGB color values (0.0-1.0 range)
	 */
	CMD_SYSTEM_LED_COLOR =					CMD_SYSTEM_OFFSET + 0x0B,
	
	/** @brief XMEGA user signature bytes
	 *  @scpi_cmd SYSTem:XMEGA:SIGnature?
	 *  @description Returns microcontroller signature bytes for identification
	 */
	CMD_SYSTEM_XMEGA_USER_SIGNATURES =		CMD_SYSTEM_OFFSET + 0x0C,
	
	/** @brief Enter sleep mode
	 *  @scpi_cmd SYSTem:SLEEP
	 *  @description Puts the device into low-power sleep mode
	 */
	CMD_SYSTEM_SLEEP =						CMD_SYSTEM_OFFSET + 0x0F,
} CMD_SYSTEM_t;
#undef CMD_SYSTEM_OFFSET

/**
 * @brief Device Under Test (DUT) command identifiers
 * @ingroup scpi_dut_commands
 * 
 * DUT commands configure and query parameters related to the device being tested.
 * These include physical characteristics, manufacturer information, and calibration data.
 */
typedef enum {
	/** @brief Junction configuration
	 *  @scpi_cmd DUT:JUNCtion[?]
	 *  @description Sets or queries DUT junction type identifier
	 */
	CMD_DUT_JUNCTION =						CMD_DUT_OFFSET + 0x00,
	
	/** @brief Coverglass configuration
	 *  @scpi_cmd DUT:COVERglass[?]
	 *  @description Sets or queries DUT coverglass type identifier
	 */
	CMD_DUT_COVERGLASS =					CMD_DUT_OFFSET + 0x01,
	
	/** @brief Interconnect configuration
	 *  @scpi_cmd DUT:INTERconnect[?]
	 *  @description Sets or queries DUT interconnect type identifier
	 */
	CMD_DUT_INTERCONNECT =					CMD_DUT_OFFSET + 0x02,
	
	/** @brief Reserved for future use */
	CMD_DUT_RESERVED =						CMD_DUT_OFFSET + 0x03,
	
	/** @brief DUT manufacturer name
	 *  @scpi_cmd DUT:MANufacturer[?]
	 *  @description Sets or queries DUT manufacturer name string
	 */
	CMD_DUT_MANUFACTURER =					CMD_DUT_OFFSET + 0x04,
	
	/** @brief DUT model designation
	 *  @scpi_cmd DUT:MODel[?]
	 *  @description Sets or queries DUT model/part number string
	 */
	CMD_DUT_MODEL =							CMD_DUT_OFFSET + 0x05,
	
	/** @brief DUT technology type
	 *  @scpi_cmd DUT:TECHnology[?]
	 *  @description Sets or queries DUT technology description (e.g., "Silicon", "GaAs")
	 */
	CMD_DUT_TECHNOLOGY =					CMD_DUT_OFFSET + 0x06,
	
	/** @brief DUT serial number
	 *  @scpi_cmd DUT:SERialnumber[?]
	 *  @description Sets or queries DUT serial number string
	 */
	CMD_DUT_SERIAL_NUMBER =					CMD_DUT_OFFSET + 0x07,
	
	/** @brief Total radiation energy exposure
	 *  @scpi_cmd DUT:ENERGY[?]
	 *  @description Sets or queries cumulative radiation energy exposure in MeV
	 */
	CMD_DUT_ENERGY =						CMD_DUT_OFFSET + 0x08,
	
	/** @brief Total radiation dose exposure
	 *  @scpi_cmd DUT:DOSE[?]
	 *  @description Sets or queries cumulative radiation dose exposure in krad
	 */
	CMD_DUT_DOSE =							CMD_DUT_OFFSET + 0x09,
	
	/** @brief DUT notes and comments
	 *  @scpi_cmd DUT:NOTEs[?]
	 *  @description Sets or queries free-form notes about the DUT
	 */
	CMD_DUT_NOTES =							CMD_DUT_OFFSET + 0x0B,
	
	/** @brief Temperature sensor type
	 *  @scpi_cmd DUT:TSENSor:TYPE[?]
	 *  @description Sets or queries temperature sensor type identifier
	 */
	CMD_DUT_TSENSOR_TYPE =					CMD_DUT_OFFSET + 0x0D,
	
	/** @brief Number of temperature sensors
	 *  @scpi_cmd DUT:TSENSor:NUMber[?]
	 *  @description Sets or queries number of temperature sensors on DUT
	 */
	CMD_DUT_TSENSOR_NUMBER =				CMD_DUT_OFFSET + 0x0E,
	
	/** @brief Temperature sensor calibration coefficients
	 *  @scpi_cmd DUT:TSENSor:FIT[?]
	 *  @description Sets or queries temperature sensor calibration polynomial coefficients
	 */
	CMD_DUT_TSENSOR_FIT =					CMD_DUT_OFFSET + 0x0F,
} CMD_DUT_t;
#undef CMD_DUT_OFFSET

/**
 * @brief Execution and measurement command identifiers
 * @ingroup scpi_measurement_commands
 * 
 * These commands trigger measurements and calibration procedures.
 * Most return measurement data or perform calibration operations.
 */
typedef enum {
	/** @brief Measure all active ADC channels
	 *  @scpi_cmd MEASure:ADC:ACTive[:RAW]?
	 *  @description Returns measurements from all currently active ADC channels
	 */
	CMD_EXEC_MEAS_ACTIVE_CHANNELS =			CMD_EXEC_OFFSET + 0x00,
	
	/** @brief Measure specific ADC channel
	 *  @scpi_cmd MEASure:ADC:CH#[:RAW]?
	 *  @description Returns measurement from specified ADC channel (0-15)
	 */
	CMD_EXEC_MEAS_CHANNEL =					CMD_EXEC_OFFSET + 0x01,
	
	/** @brief Measure all temperature sensors
	 *  @scpi_cmd MEASure:ADC:TSENSORS[:RAW]?
	 *  @description Returns temperature measurements from all configured sensors
	 */
	CMD_EXEC_MEAS_TSENSORS =				CMD_EXEC_OFFSET + 0x02,
	
	/** @brief Measure internal voltages
	 *  @scpi_cmd MEASure:INTERNALvoltages?
	 *  @description Returns internal supply voltage measurements (AVDD, IOVDD, etc.)
	 */
	CMD_EXEC_MEAS_INTERNAL_VOLTAGES =		CMD_EXEC_OFFSET + 0x03,
	
	/** @brief Measure sun sensor angles
	 *  @scpi_cmd MEASure:SUNSensor?
	 *  @description Returns calculated sun sensor yaw and pitch angles
	 */
	CMD_EXEC_MEAS_SUN_SENSOR =				CMD_EXEC_OFFSET + 0x04,
	
	/** @brief Measure pressure sensor
	 *  @scpi_cmd MEASure:PRESSure?
	 *  @description Returns pressure sensor measurement data
	 */
	CMD_EXEC_MEAS_PRESSURE_SENSOR =			CMD_EXEC_OFFSET + 0x05,
	
	/** @brief Perform ADC calibration
	 *  @scpi_cmd ADC:CALibrate[?]
	 *  @description Initiates ADC calibration procedure and returns calibration value
	 */
	CMD_EXEC_ADC_CAL =						CMD_EXEC_OFFSET + 0x07,
	
	/** @brief Calibrate all internal ADC references
	 *  @scpi_cmd ADC:CALibrate:ALL:INTernal
	 *  @description Performs calibration on all internal ADC reference channels
	 */
	CMD_EXEC_ADC_CAL_ALL_INTERNAL =			CMD_EXEC_OFFSET + 0x08,
	
	/** @brief Save all internal ADC calibrations
	 *  @scpi_cmd ADC:SAVE:ALL:INTernal
	 *  @description Saves all internal ADC calibration data to EEPROM
	 */
	CMD_EXEC_ADC_CAL_SAVE_ALL_INTERNAL =	CMD_EXEC_OFFSET + 0x09,
	
	/** @brief Perform DAC calibration
	 *  @scpi_cmd DAC:CALibrate[?]
	 *  @description Initiates DAC calibration procedure and returns calibration value
	 */
	CMD_EXEC_DAC_CAL =						CMD_EXEC_OFFSET + 0x0A,
	
	/** @brief Save DAC calibration
	 *  @scpi_cmd DAC:CALibrate:SAVe
	 *  @description Saves DAC calibration data to EEPROM
	 */
	CMD_EXEC_DAC_CAL_SAVE =					CMD_EXEC_OFFSET + 0x0B,
	
	/** @brief Calibrate temperature sensor at 25°C
	 *  @scpi_cmd ADC:CALibrate:TSENSor
	 *  @description Performs temperature sensor calibration at 25°C reference
	 */
	CMD_EXEC_TSENSOR_CAL_25C =				CMD_EXEC_OFFSET + 0x0C,
	
	/** @brief Save temperature sensor coefficients
	 *  @scpi_cmd DUT:TSENSor:FIT:SAVE (via CMD_EXEC_TSENSOR_COEFF_SAVE)
	 *  @description Saves temperature sensor calibration coefficients to EEPROM
	 */
	CMD_EXEC_TSENSOR_COEFF_SAVE =			CMD_EXEC_OFFSET + 0x0D,
	
	/** @brief Save sun sensor coefficients
	 *  @scpi_cmd SUNSensor:FIT:SAVE
	 *  @description Saves sun sensor calibration coefficients to EEPROM
	 */
	CMD_EXEC_SUNSENSOR_COEFF_SAVE =			CMD_EXEC_OFFSET + 0x0E,
	
	/** @brief Save heater PID parameters
	 *  @scpi_cmd HEATer:PID:SAVE
	 *  @description Saves heater PID controller parameters to EEPROM
	 */
	CMD_EXEC_HEATER_PID_SAVE  =				CMD_EXEC_OFFSET + 0x0F,
} CMD_EXEC_t;
#undef CMD_EXEC_OFFSET

/**
 * @brief I-V sweep control command identifiers
 * @ingroup scpi_sweep_commands
 * 
 * These commands control current-voltage sweep operations for solar cell characterization.
 * Sweeps can be configured, triggered, and data can be stored/retrieved from EEPROM.
 */
typedef enum {
	/** @brief Disable sweep operations
	 *  @scpi_cmd SWEEP:DISable
	 *  @description Disables sweep functionality and stops any ongoing sweep
	 */
	CMD_SWEEP_DISABLE =						CMD_SWEEP_OFFSET + 0x00,
	
	/** @brief Enable sweep operations
	 *  @scpi_cmd SWEEP:ENAble
	 *  @description Enables sweep functionality for I-V measurements
	 */
	CMD_SWEEP_ENABLE =						CMD_SWEEP_OFFSET + 0x01,
	
	/** @brief Trigger I-V sweep
	 *  @scpi_cmd SWEEP:TRIGger
	 *  @description Initiates a complete I-V sweep measurement
	 */
	CMD_SWEEP_TRIG_SWEEP =					CMD_SWEEP_OFFSET + 0x02,
	
	/** @brief Measure short-circuit current (Isc)
	 *  @scpi_cmd SWEEP:TRIGger:ISC?
	 *  @description Measures and returns short-circuit current value
	 */
	CMD_SWEEP_TRIG_ISC =					CMD_SWEEP_OFFSET + 0x03,
	
	/** @brief Measure open-circuit voltage (Voc)
	 *  @scpi_cmd SWEEP:TRIGger:VOC?
	 *  @description Measures and returns open-circuit voltage value
	 */
	CMD_SWEEP_TRIG_VOC =					CMD_SWEEP_OFFSET + 0x04,
	
	/** @brief Save sweep configuration
	 *  @scpi_cmd SWEEP:CONFig:SAVe
	 *  @description Saves current sweep configuration to EEPROM
	 */
	CMD_SWEEP_CONFIG_SAVE =					CMD_SWEEP_OFFSET + 0x05,
	
	/** @brief Save I-V data to EEPROM
	 *  @scpi_cmd SWEEP:EEPROM:SAVE
	 *  @description Saves sweep measurement data to EEPROM
	 */
	CMD_SWEEP_IV_SAVE_TO_EEPROM =			CMD_SWEEP_OFFSET + 0x06,
	
	/** @brief Load I-V data from EEPROM
	 *  @scpi_cmd SWEEP:EEPROM:LOAD
	 *  @description Loads previously saved sweep data from EEPROM
	 */
	CMD_SWEEP_IV_LOAD_FROM_EEPROM =			CMD_SWEEP_OFFSET + 0x07,
	
	/** @brief Encrypt sweep data using AES
	 *  @scpi_cmd SWEEP:AES:ENCode
	 *  @description Encrypts sweep data using AES encryption
	 */
	CMD_SWEEP_AES_ENCODE =					CMD_SWEEP_OFFSET + 0x08,
	
	/** @brief Decrypt sweep data using AES
	 *  @scpi_cmd SWEEP:AES:DECode
	 *  @description Decrypts AES-encrypted sweep data
	 */
	CMD_SWEEP_AES_DECODE =					CMD_SWEEP_OFFSET + 0x09,
	
	/** @brief Save single sweep data point
	 *  @scpi_cmd SWEEP:DATApoint:SAVE
	 *  @description Saves a single voltage/current data point
	 */
	CMD_SWEEP_DATAPOINT_SAVE =				CMD_SWEEP_OFFSET + 0x0A,
	
	/** @brief Initialize sweep trigger system
	 *  @scpi_cmd SWEEP:TRIGger:INITialize
	 *  @description Initializes the sweep trigger system
	 */
	CMD_SWEEP_TRIG_INIT =					CMD_SWEEP_OFFSET + 0x0B,
	
	/** @brief Load single sweep data point
	 *  @scpi_cmd SWEEP:DATApoint:LOAD
	 *  @description Loads a single voltage/current data point
	 */
	CMD_SWEEP_DATAPOINT_LOAD =				CMD_SWEEP_OFFSET + 0x0C,
} CMD_SWEEP_t;
#undef CMD_SWEEP_OFFSET

/**
 * @brief Auxiliary subsystem command identifiers
 * @ingroup scpi_dac_commands scpi_heater_commands scpi_sunsensor_commands
 * 
 * These commands control auxiliary subsystems including DAC output, heater control,
 * and sun sensor calibration. Commands support both raw values and calibrated units.
 */
typedef enum {	
	/** @brief DAC output state control
	 *  @scpi_cmd DAC:STATE[?]
	 *  @description Sets or queries DAC output enable state (0=disabled, 1=enabled)
	 */
	CMD_AUX_DAC_STATE =						CMD_AUX_OFFSET + 0x00,
	
	/** @brief DAC current output setting
	 *  @scpi_cmd DAC:CURRent[?]
	 *  @description Sets or queries DAC current output in amperes
	 */
	CMD_AUX_DAC_CURRENT =					CMD_AUX_OFFSET + 0x01,
	
	/** @brief DAC current raw value
	 *  @scpi_cmd DAC:CURRent:RAW[?]
	 *  @description Sets or queries DAC current output as raw 16-bit value
	 */
	CMD_AUX_DAC_CURRENT_RAW =				CMD_AUX_OFFSET + 0x02,
	
	/** @brief DAC voltage output setting
	 *  @scpi_cmd DAC:VOLTage[?]
	 *  @description Sets or queries DAC voltage output in volts
	 */
	CMD_AUX_DAC_VOLTAGE =					CMD_AUX_OFFSET + 0x03,
	
	/** @brief DAC voltage raw value
	 *  @scpi_cmd DAC:VOLTage:RAW[?]
	 *  @description Sets or queries DAC voltage output as raw 16-bit value
	 */
	CMD_AUX_DAC_VOLTAGE_RAW =				CMD_AUX_OFFSET + 0x04,
	
	/** @brief DAC offset value
	 *  @scpi_cmd DAC:OFFset[?]
	 *  @description Sets or queries DAC offset calibration value
	 */
	CMD_AUX_DAC_OFFSET =					CMD_AUX_OFFSET + 0x05,
	
	/** @brief DAC offset correction factor
	 *  @scpi_cmd DAC:OFFset:CORRection[?]
	 *  @description Sets or queries DAC offset correction coefficient
	 */
	CMD_AUX_DAC_OFFSET_CORRECTION =			CMD_AUX_OFFSET + 0x06,
	
	/** @brief DAC gain correction factor
	 *  @scpi_cmd DAC:GAIN:CORRection[?]
	 *  @description Sets or queries DAC gain correction coefficient
	 */
	CMD_AUX_DAC_GAIN_CORRECTION =			CMD_AUX_OFFSET + 0x07,
	
	/** @brief Heater enable state
	 *  @scpi_cmd HEATer:STATE[?]
	 *  @description Sets or queries heater enable state (0=off, 1=on)
	 */
	CMD_AUX_HEATER_STATE =					CMD_AUX_OFFSET + 0x08,
	
	/** @brief Heater temperature setpoint
	 *  @scpi_cmd HEATer:SETpoint[?]
	 *  @description Sets or queries heater temperature setpoint in Celsius
	 */
	CMD_AUX_HEATER_SETPOINT =				CMD_AUX_OFFSET + 0x09,
	
	/** @brief Heater PID controller parameters
	 *  @scpi_cmd HEATer:PID[?]
	 *  @description Sets or queries heater PID controller coefficients (Kp, Ki, Kd)
	 */
	CMD_AUX_HEATER_PID =					CMD_AUX_OFFSET + 0x0A,
	
	/** @brief Sun sensor yaw calibration coefficients
	 *  @scpi_cmd SUNSensor:FIT:YAW[?]
	 *  @description Sets or queries sun sensor yaw angle calibration polynomial coefficients
	 */
	CMD_AUX_SUNSENSOR_FIT_YAW_COEFF =		CMD_AUX_OFFSET + 0x0B,
	
	/** @brief Sun sensor pitch calibration coefficients
	 *  @scpi_cmd SUNSensor:FIT:PITCH[?]
	 *  @description Sets or queries sun sensor pitch angle calibration polynomial coefficients
	 */
	CMD_AUX_SUNSENSOR_FIT_PITCH_COEFF =		CMD_AUX_OFFSET + 0x0C,
	
	/** @brief Sun sensor horizontal reference value
	 *  @scpi_cmd SUNSensor:HVAL[?]
	 *  @description Sets or queries sun sensor horizontal reference calibration value
	 */
	CMD_AUX_SUNSENSOR_HVAL =				CMD_AUX_OFFSET + 0x0D,
	
	/** @brief Sun sensor radial reference value
	 *  @scpi_cmd SUNSensor:RVAL[?]
	 *  @description Sets or queries sun sensor radial reference calibration value
	 */
	CMD_AUX_SUNSENSOR_RVAL =				CMD_AUX_OFFSET + 0x0E,
	
	/** @brief Sun sensor detection threshold
	 *  @scpi_cmd SUNSensor:THRESHold[?]
	 *  @description Sets or queries sun sensor detection threshold value
	 */
	CMD_AUX_SUNSENSOR_THRESHOLD =			CMD_AUX_OFFSET + 0x0F,	
} CMD_AUX_t;
#undef CMD_AUX_OFFSET

/**
 * @brief ADC channel configuration command identifiers
 * @ingroup scpi_adc_commands
 * 
 * These commands configure individual ADC channels including setup, filtering,
 * gain settings, and calibration procedures. Channel numbers range from 0-15.
 */
typedef enum {
	/** @brief ADC channel register value
	 *  @scpi_cmd ADC:CH#[?]
	 *  @description Sets or queries ADC channel register configuration
	 */
	CMD_ADC_CH_REG =						CMD_ADC_CH_OFFSET + 0x00,
	
	/** @brief ADC channel setup configuration
	 *  @scpi_cmd ADC:CH#:SETup[?]
	 *  @description Sets or queries ADC channel setup register (input selection, etc.)
	 */
	CMD_ADC_CH_SETUP =						CMD_ADC_CH_OFFSET + 0x01,
	
	/** @brief ADC channel filter configuration
	 *  @scpi_cmd ADC:CH#:FILTer[?]
	 *  @description Sets or queries ADC channel digital filter settings
	 */
	CMD_ADC_CH_FILTER =						CMD_ADC_CH_OFFSET + 0x02,
	
	/** @brief ADC channel sample rate
	 *  @scpi_cmd ADC:CH#:RATE[?]
	 *  @description Sets or queries ADC channel sample rate in Hz
	 */
	CMD_ADC_CH_RATE =						CMD_ADC_CH_OFFSET + 0x03,
	
	/** @brief ADC channel PGA setting
	 *  @scpi_cmd ADC:CH#:PGA[?]
	 *  @description Sets or queries programmable gain amplifier setting (1, 2, 4, 8, 16, 32, 64, 128)
	 */
	CMD_ADC_CH_PGA =						CMD_ADC_CH_OFFSET + 0x04,
	
	/** @brief ADC channel maximum input range
	 *  @scpi_cmd ADC:CH#:MAX?
	 *  @description Queries maximum input range for current PGA setting
	 */
	CMD_ADC_CH_PGA_MAX =					CMD_ADC_CH_OFFSET + 0x05,
	
	/** @brief ADC channel voltage maximum for PGA
	 *  @scpi_cmd ADC:VOLTage:MAX:PGA#?
	 *  @description Queries maximum voltage range for specified PGA setting
	 */
	CMD_ADC_CH_PGA_VMAX =					CMD_ADC_CH_OFFSET + 0x06,
	
	/** @brief ADC channel current maximum for PGA
	 *  @scpi_cmd ADC:CURRent:MAX:PGA#?
	 *  @description Queries maximum current range for specified PGA setting
	 */
	CMD_ADC_CH_PGA_IMAX =					CMD_ADC_CH_OFFSET + 0x07,
	
	/** @brief Save ADC channel configuration
	 *  @scpi_cmd ADC:CH#:SAVE
	 *  @description Saves current channel configuration to EEPROM
	 */
	CMD_ADC_CH_SAVE =						CMD_ADC_CH_OFFSET + 0x08,
	
	/** @brief ADC channel offset calibration coefficient
	 *  @scpi_cmd ADC:CH#:OFFset[?]
	 *  @description Sets or queries ADC channel offset calibration coefficient
	 */
	CMD_ADC_CH_OFFSET_COEFF =				CMD_ADC_CH_OFFSET + 0x09,
	
	/** @brief ADC channel gain calibration coefficient
	 *  @scpi_cmd ADC:CH#:GAIN[?]
	 *  @description Sets or queries ADC channel gain calibration coefficient
	 */
	CMD_ADC_CH_GAIN_COEFF =					CMD_ADC_CH_OFFSET + 0x0A,
	
	/** @brief Perform internal ADC calibration
	 *  @scpi_cmd ADC:CH#:CALibrate:INTernal
	 *  @description Performs internal calibration for the specified channel
	 */
	CMD_ADC_CH_CAL_INTERNAL =				CMD_ADC_CH_OFFSET + 0x0B,
	
	/** @brief Perform zero-scale calibration
	 *  @scpi_cmd ADC:CH#:CALibrate:ZERO
	 *  @description Performs zero-scale calibration for the specified channel
	 */
	CMD_ADC_CH_CAL_ZERO_SCALE =				CMD_ADC_CH_OFFSET + 0x0C,
	
	/** @brief Perform full-scale calibration
	 *  @scpi_cmd ADC:CH#:CALibrate:FULL
	 *  @description Performs full-scale calibration for the specified channel
	 */
	CMD_ADC_CH_CAL_FULL_SCALE =				CMD_ADC_CH_OFFSET + 0x0D,
	
	/** @brief Reset channel calibration
	 *  @scpi_cmd ADC:CH#:CALibrate:RESet
	 *  @description Resets channel calibration to factory defaults
	 */
	CMD_ADC_CH_CAL_RESET =					CMD_ADC_CH_OFFSET + 0x0E,
	
	/** @brief Save channel calibration
	 *  @scpi_cmd ADC:CH#:CALibrate:SAVe
	 *  @description Saves channel calibration coefficients to EEPROM
	 */
	CMD_ADC_CH_CAL_SAVE =					CMD_ADC_CH_OFFSET + 0x0F,
} CMD_ADC_CH_t;
#undef CMD_ADC_CH_OFFSET

/**
 * @brief Measurement channel command identifiers
 * @ingroup scpi_measurement_commands
 * 
 * These commands perform measurements on specific ADC channels.
 * Channel assignments are based on AMU_ADC_CH_t definitions.
 */
typedef enum {
	/** @brief Measure voltage channel
	 *  @scpi_cmd MEASure:ADC:VOLTage[:RAW]?
	 *  @description Measures voltage on the dedicated voltage input channel
	 */
	CMD_MEAS_CH_VOLTAGE =					CMD_MEAS_CH_CMD_OFFSET + AMU_ADC_CH_VOLTAGE,
	
	/** @brief Measure current channel
	 *  @scpi_cmd MEASure:ADC:CURRent[:RAW]?
	 *  @description Measures current on the dedicated current input channel
	 */
	CMD_MEAS_CH_CURRENT =					CMD_MEAS_CH_CMD_OFFSET + AMU_ADC_CH_CURRENT,
	
	/** @brief Measure primary temperature sensor
	 *  @scpi_cmd MEASure:ADC:TSENSor[:RAW]?
	 *  @description Measures primary temperature sensor (same as TSENSOR0)
	 */
	CMD_MEAS_CH_TSENSOR =					CMD_MEAS_CH_CMD_OFFSET + AMU_ADC_CH_TSENSOR0,
	
	/** @brief Measure temperature sensor 0
	 *  @scpi_cmd MEASure:ADC:TSENSOR0[:RAW]?
	 *  @description Measures temperature sensor 0
	 */
	CMD_MEAS_CH_TSENSOR_0 =					CMD_MEAS_CH_CMD_OFFSET + AMU_ADC_CH_TSENSOR0,
	
	/** @brief Measure temperature sensor 1
	 *  @scpi_cmd MEASure:ADC:TSENSOR1[:RAW]?
	 *  @description Measures temperature sensor 1
	 */
	CMD_MEAS_CH_TSENSOR_1 =					CMD_MEAS_CH_CMD_OFFSET + AMU_ADC_CH_TSENSOR1,
	
	/** @brief Measure temperature sensor 2
	 *  @scpi_cmd MEASure:ADC:TSENSOR2[:RAW]?
	 *  @description Measures temperature sensor 2
	 */
	CMD_MEAS_CH_TSENSOR_2 =					CMD_MEAS_CH_CMD_OFFSET + AMU_ADC_CH_TSENSOR2,
	
	/** @brief Measure bias voltage
	 *  @scpi_cmd MEASure:ADC:BIAS[:RAW]?
	 *  @description Measures bias voltage reference
	 */
	CMD_MEAS_CH_BIAS =						CMD_MEAS_CH_CMD_OFFSET + AMU_ADC_CH_BIAS,
	
	/** @brief Measure offset reference
	 *  @scpi_cmd MEASure:ADC:OFFset[:RAW]?
	 *  @description Measures offset calibration reference
	 */
	CMD_MEAS_CH_OFFSET =					CMD_MEAS_CH_CMD_OFFSET + AMU_ADC_CH_OFFSET,
	
	/** @brief Measure internal temperature
	 *  @scpi_cmd MEASure:ADC:TEMP[:RAW]?
	 *  @description Measures internal MCU temperature sensor
	 */
	CMD_MEAS_CH_TEMP =						CMD_MEAS_CH_CMD_OFFSET + AMU_ADC_CH_TEMP,
	
	/** @brief Measure analog supply voltage
	 *  @scpi_cmd MEASure:ADC:AVDD[:RAW]?
	 *  @description Measures analog supply voltage (AVDD)
	 */
	CMD_MEAS_CH_AVDD =						CMD_MEAS_CH_CMD_OFFSET + AMU_ADC_CH_AVDD,
	
	/** @brief Measure I/O supply voltage
	 *  @scpi_cmd MEASure:ADC:IOVDD[:RAW]?
	 *  @description Measures I/O supply voltage (IOVDD)
	 */
	CMD_MEAS_CH_IOVDD =						CMD_MEAS_CH_CMD_OFFSET + AMU_ADC_CH_IOVDD,
	
	/** @brief Measure analog LDO voltage
	 *  @scpi_cmd MEASure:ADC:ALDO[:RAW]?
	 *  @description Measures analog LDO regulator output voltage
	 */
	CMD_MEAS_CH_ALDO =						CMD_MEAS_CH_CMD_OFFSET + AMU_ADC_CH_ALDO,
	
	/** @brief Measure digital LDO voltage
	 *  @scpi_cmd MEASure:ADC:DLDO[:RAW]?
	 *  @description Measures digital LDO regulator output voltage
	 */
	CMD_MEAS_CH_DLDO =						CMD_MEAS_CH_CMD_OFFSET + AMU_ADC_CH_DLDO,
	
	/** @brief Measure sun sensor top-left quadrant
	 *  @scpi_cmd MEASure:ADC:SSTL[:RAW]?
	 *  @description Measures sun sensor top-left quadrant photodiode
	 */
	CMD_MEAS_CH_SS_TL =						CMD_MEAS_CH_CMD_OFFSET + AMU_ADC_CH_SS_TL,
	
	/** @brief Measure sun sensor bottom-left quadrant
	 *  @scpi_cmd MEASure:ADC:SSBL[:RAW]?
	 *  @description Measures sun sensor bottom-left quadrant photodiode
	 */
	CMD_MEAS_CH_SS_BL=						CMD_MEAS_CH_CMD_OFFSET + AMU_ADC_CH_SS_BL,
	
	/** @brief Measure sun sensor bottom-right quadrant
	 *  @scpi_cmd MEASure:ADC:SSBR[:RAW]?
	 *  @description Measures sun sensor bottom-right quadrant photodiode
	 */
	CMD_MEAS_CH_SS_BR =						CMD_MEAS_CH_CMD_OFFSET + AMU_ADC_CH_SS_BR,
	
	/** @brief Measure sun sensor top-right quadrant
	 *  @scpi_cmd MEASure:ADC:SSTR[:RAW]?
	 *  @description Measures sun sensor top-right quadrant photodiode
	 */
	CMD_MEAS_CH_SS_TR =						CMD_MEAS_CH_CMD_OFFSET + AMU_ADC_CH_SS_TR,
} CMD_MEAS_CH_t;
#undef CMD_MEAS_CH_CMD_OFFSET


/**
 * @brief USB-only system command identifiers
 * @ingroup scpi_system_commands
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
 * @ingroup scpi_memory_commands
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
 * @ingroup scpi_adc_commands
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
	 */
	CMD_USB_ADC_VOLTAGE_OFFSET =		CMD_USB_ADC_VOLTAGE_CMD_OFFSET + 0x06,
	
	/** @brief Voltage channel gain coefficient
	 *  @scpi_cmd ADC:VOLTage:GAIN[?]
	 *  @description Sets or queries voltage channel gain coefficient (USB only)
	 */
	CMD_USB_ADC_VOLTAGE_GAIN =			CMD_USB_ADC_VOLTAGE_CMD_OFFSET + 0x07,
} CMD_USB_ADC_VOLTAGE_t;
#undef CMD_USB_ADC_VOLTAGE_CMD_OFFSET


/**
 * @brief USB-only current ADC command identifiers
 * @ingroup scpi_adc_commands
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
	 */
	CMD_USB_ADC_CURRENT_OFFSET =		CMD_USB_ADC_CURRENT_CMD_OFFSET + 0x06,
	
	/** @brief Current channel gain coefficient
	 *  @scpi_cmd ADC:CURRent:GAIN[?]
	 *  @description Sets or queries current channel gain coefficient (USB only)
	 */
	CMD_USB_ADC_CURRENT_GAIN =			CMD_USB_ADC_CURRENT_CMD_OFFSET + 0x07,
} CMD_USB_ADC_CURRENT_t;
#undef CMD_USB_ADC_CURRENT_CMD_OFFSET

/**
 * @brief USB-only sweep command identifiers
 * @ingroup scpi_sweep_commands
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
 * @ingroup scpi_system_commands
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