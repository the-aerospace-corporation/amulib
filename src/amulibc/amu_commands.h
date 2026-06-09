/**
 * @file amu_commands.h
 * @brief AMU Command Definitions and SCPI Command Mappings
 *
 * This file defines all command IDs used by the AMU (Aerospace Measurement Unit) for both
 * I2C and USB communication interfaces. Each command is mapped to specific SCPI strings
 * in scpi.h to provide a standardized instrument control interface.
 * 
 * Commands use offset-based grouping: each group is a 16-command block, the high bit of the
 * low byte flags read vs. write, and the high byte selects the interface root (CMD_I2C_USB
 * for I2C+USB commands, CMD_USB_ONLY for USB-only). The groups themselves are defined as the
 * @c cmd_* Doxygen groups below.
 *
 * @author	CJM28241
 * @date	10/25/2018 10:47:55 PM
 */ 

#ifndef __AMU_COMMANDS_H__
#define __AMU_COMMANDS_H__

#include "amu_types.h"

/** @defgroup cmd_system System Commands
 *  @brief Device control, status, identification, LED, and timestamps. */
/** @defgroup cmd_dut DUT Commands
 *  @brief Device-under-test configuration and metadata. */
/** @defgroup cmd_exec Execution & Calibration
 *  @brief Measurement triggers and calibration procedures. */
/** @defgroup cmd_sweep Sweep Commands
 *  @brief I-V sweep control, data, and metadata. */
/** @defgroup cmd_aux Auxiliary Commands
 *  @brief DAC, heater, and sun sensor control. */
/** @defgroup cmd_adc ADC Channel Commands
 *  @brief Per-channel ADC configuration and calibration. */
/** @defgroup cmd_meas Measurement Commands
 *  @brief Channel measurement reads. */
/** @defgroup cmd_memory Memory Commands
 *  @brief EEPROM calibration storage (USB only). */

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

#define CMD_I2C_USB				0x0100		// Root command for I2C/USB commands
#define CMD_USB_ONLY			0x0200		// Root command for USB only commands

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
 * @ingroup cmd_system
 *
 * System commands control basic device functionality and provide device information.
 * These I2C commands are available on both I2C and USB interfaces.
 * Many have corresponding SCPI command equivalents.
 */
typedef enum {
	/** @amutitle{System — Self-Test}
	 *  @amudesc{No operation command - communication test}
	 *  @return Status code (0 = success)
	 *
	 *  @amupanels
	 *  @amuscpi{*TST?}
	 *  @amupanelex
	 *  *TST?
	 *  0
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_SYSTEM_NO_CMD}
	 *  @amupanelex
	 *  amu.sendCommand(CMD_SYSTEM_NO_CMD);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_SYSTEM_NO_CMD =						CMD_SYSTEM_OFFSET + 0x00,
	
	/** @amutitle{System — Reset}
	 *  @amudesc{Performs complete software reset}
	 *
	 *  @amupanels
	 *  @amuscpi{*RST}
	 *  @amupanelex
	 *  *RST
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_SYSTEM_RESET}
	 *  @amupanelex
	 *  amu.sendCommand(CMD_SYSTEM_RESET);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 *  @warning All user configuration will be lost.
	 *  @note Device will disconnect briefly during the reset process.
	 */
	CMD_SYSTEM_RESET =						CMD_SYSTEM_OFFSET + 0x01,
	
	/** @amutitle{System — XMEGA Fuses}
	 *  @amudesc{Returns microcontroller fuse configuration}
	 *  @return 32-bit fuse configuration (hex format: 0xAABBCCDD)
	 *
	 *  @amupanels
	 *  @amuscpi{SYSTem:XMEGA:FUSES?}
	 *  @amupanelex
	 *  SYSTem:XMEGA:FUSES?
	 *  0xFF00FFCC
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_SYSTEM_XMEGA_FUSES}
	 *  @amupanelex
	 *  uint32_t fuses = amu.query<uint32_t>(CMD_SYSTEM_XMEGA_FUSES);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 *  @note Read-only command for diagnostic purposes.
	 */
	CMD_SYSTEM_XMEGA_FUSES =				CMD_SYSTEM_OFFSET + 0x02,
	
	/** @amutitle{System — TWI Address}
	 *  @amudesc{Sets or queries I2C slave address}
	 *  @param address 7-bit I2C address (range: 0x08-0x77, excludes reserved addresses)
	 *  @return Current I2C address (7-bit, no R/W bit)
	 *
	 *  @amupanels
	 *  @amuscpi{SYSTem:TWI:ADDress[?]}
	 *  @amupanelex
	 *  SYSTem:TWI:ADDress 0x42
	 *  SYSTem:TWI:ADDress?
	 *  66
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_SYSTEM_TWI_ADDRESS}
	 *  @amupanelex
	 *  uint8_t addr = amu.query<uint8_t>(CMD_SYSTEM_TWI_ADDRESS);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 *  @warning Addresses 0x00-0x07 and 0x78-0x7F are reserved and will be rejected.
	 *  @note Address changes take effect immediately but are not saved to EEPROM.
	 */
	CMD_SYSTEM_TWI_ADDRESS =				CMD_SYSTEM_OFFSET + 0x03,
	
	/** @amutitle{System — TWI Device Count}
	 *  @amudesc{Scans I2C bus and counts devices}
	 *  @return Number of detected I2C devices (0-112)
	 *
	 *  @amupanels
	 *  @amuscpi{SYSTem:TWI:NUMdevices?}
	 *  @amupanelex
	 *  SYSTem:TWI:NUMdevices?
	 *  3
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_SYSTEM_TWI_NUM_DEVICES}
	 *  @amupanelex
	 *  uint8_t n = amu.query<uint8_t>(CMD_SYSTEM_TWI_NUM_DEVICES);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 *  @note Scan may take up to 2 seconds to complete.
	 */
	CMD_SYSTEM_TWI_NUM_DEVICES =			CMD_SYSTEM_OFFSET + 0x04,
	
	/** @amutitle{System — TWI Status}
	 *  @amudesc{Returns I2C interface status}
	 *  @return Status bitfield: [7:4]=Reserved [3]=Bus_Error [2]=Arbitration_Lost [1]=NACK [0]=Active
	 *
	 *  @amupanels
	 *  @amuscpi{SYSTem:TWI:STATus?}
	 *  @amupanelex
	 *  SYSTem:TWI:STATus?
	 *  0
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_SYSTEM_TWI_STATUS}
	 *  @amupanelex
	 *  uint8_t status = amu.query<uint8_t>(CMD_SYSTEM_TWI_STATUS);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 *  @note Bitfield: [7:4]=Reserved [3]=Bus_Error [2]=Arbitration_Lost [1]=NACK [0]=Active. Status bits are cleared after reading.
	 */
	CMD_SYSTEM_TWI_STATUS =					CMD_SYSTEM_OFFSET + 0x05,
	
	/** @amutitle{System — Firmware}
	 *  @amudesc{Returns firmware version string}
	 *  @return Version string (format: "X.Y.Z", e.g., "2.1.0")
	 *
	 *  @amupanels
	 *  @amuscpi{SYSTem:FIRMware?}
	 *  @amupanelex
	 *  SYSTem:FIRMware?
	 *  3.0.1
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_SYSTEM_FIRMWARE}
	 *  @amupanelex
	 *  char fw[AMU_FIRMWARE_STR_LEN]; amu.query<char>(CMD_SYSTEM_FIRMWARE, fw, sizeof(fw));
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 *  @note Maximum string length: 16 characters.
	 */
	CMD_SYSTEM_FIRMWARE =					CMD_SYSTEM_OFFSET + 0x06,
	
	/** @amutitle{System — Serial}
	 *  @amudesc{Returns unique device serial number}
	 *  @return Serial number string (format: "AMU-YYYYMMDD-XXXX")
	 *
	 *  @amupanels
	 *  @amuscpi{SYSTem:SERial?}
	 *  @amupanelex
	 *  SYSTem:SERial?
	 *  AMU-20240115-0042
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_SYSTEM_SERIAL_NUM}
	 *  @amupanelex
	 *  char sn[AMU_SERIALNUM_STR_LEN]; amu.query<char>(CMD_SYSTEM_SERIAL_NUM, sn, sizeof(sn));
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 *  @note Serial numbers are globally unique across all devices.
	 */
	CMD_SYSTEM_SERIAL_NUM =					CMD_SYSTEM_OFFSET + 0x07,
	
	/** @amutitle{System — Temperature}
	 *  @amudesc{Internal MCU temperature sensor}
	 *  @return Temperature in degrees Celsius (range: -40 to +85°C)
	 *
	 *  @amupanels
	 *  @amuscpi{SYSTem:TEMPerature?}
	 *  @amupanelex
	 *  SYSTem:TEMPerature?
	 *  23.5
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_SYSTEM_TEMPERATURE}
	 *  @amupanelex
	 *  float tempC = amu.query<float>(CMD_SYSTEM_TEMPERATURE);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 *  @note Sensor is factory calibrated at 25°C (accuracy ±3°C).
	 *  @warning High temperatures may indicate thermal stress.
	 */
	CMD_SYSTEM_TEMPERATURE =				CMD_SYSTEM_OFFSET + 0x08,
	
	/** @amutitle{System — Timestamp}
	 *  @amudesc{Sets or queries system timestamp}
	 *  @param timestamp Seconds since boot (32-bit unsigned, rolls over at ~136 years)
	 *  @return Current timestamp in seconds
	 *
	 *  @amupanels
	 *  @amuscpi{SYSTem:TIMEstamp[?]}
	 *  @amupanelex
	 *  SYSTem:TIMEstamp?
	 *  12345
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_SYSTEM_TIME}
	 *  @amupanelex
	 *  uint32_t t = amu.query<uint32_t>(CMD_SYSTEM_TIME);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 *  @note Timestamp is reset to 0 on power cycle or reset; rolls over at ~136 years.
	 */
	CMD_SYSTEM_TIME =						CMD_SYSTEM_OFFSET + 0x09,
	
	/** @amutitle{System — Timestamp UTC}
	 *  @amudesc{Sets or queries UTC timestamp}
	 *  @param utc_time Unix epoch timestamp (32-bit, valid until 2038)
	 *  @return Current UTC timestamp
	 *
	 *  @amupanels
	 *  @amuscpi{SYSTem:TIMEstamp:UTC[?]}
	 *  @amupanelex
	 *  SYSTem:TIMEstamp:UTC?
	 *  1705312200
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_SYSTEM_UTC_TIME}
	 *  @amupanelex
	 *  uint32_t utc = amu.query<uint32_t>(CMD_SYSTEM_UTC_TIME);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 *  @note Unix epoch format (seconds since Jan 1, 1970); valid until 2038. Not maintained across power cycles without an external RTC.
	 */
	CMD_SYSTEM_UTC_TIME =					CMD_SYSTEM_OFFSET + 0x0A,
	
	/** @amutitle{System — LED Color}
	 *  @amudesc{Sets or queries status LED color}
	 *  @param red Red component intensity (range: 0.0-1.0)
	 *  @param green Green component intensity (range: 0.0-1.0)
	 *  @param blue Blue component intensity (range: 0.0-1.0)
	 *  @return Current RGB values as comma-separated floats
	 *
	 *  @amupanels
	 *  @amuscpi{SYSTem:LED:COLOR[?]}
	 *  @amupanelex
	 *  SYSTem:LED:COLOR 1.0,0.0,0.5
	 *  SYSTem:LED:COLOR?
	 *  1.000,0.000,0.500
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_SYSTEM_LED_COLOR}
	 *  @amupanelex
	 *  float colors[3] = {0.0f, 1.0f, 0.0f}; amu.sendCommand(CMD_SYSTEM_LED_COLOR, colors, sizeof(colors));
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 *  @note Components are comma-separated floats; LED brightness is automatically adjusted for optimal visibility.
	 */
	CMD_SYSTEM_LED_COLOR =					CMD_SYSTEM_OFFSET + 0x0B,
	
	/** @amutitle{System — XMEGA Signature}
	 *  @amudesc{Returns microcontroller signature bytes}
	 *  @return 3-byte signature (format: 0xAABBCC)
	 *
	 *  @amupanels
	 *  @amuscpi{SYSTem:XMEGA:SIGnature?}
	 *  @amupanelex
	 *  SYSTem:XMEGA:SIGnature?
	 *  0x1E9842
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_SYSTEM_XMEGA_USER_SIGNATURES}
	 *  @amupanelex
	 *  uint32_t sig = amu.query<uint32_t>(CMD_SYSTEM_XMEGA_USER_SIGNATURES);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 *  @note Signature is factory-programmed and cannot be modified.
	 */
	CMD_SYSTEM_XMEGA_USER_SIGNATURES =		CMD_SYSTEM_OFFSET + 0x0C,
	
	/** @amutitle{System — Sleep}
	 *  @amudesc{Enters low-power sleep mode}
	 *
	 *  @amupanels
	 *  @amuscpi{SYSTem:SLEEP}
	 *  @amupanelex
	 *  SYSTem:SLEEP
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_SYSTEM_SLEEP}
	 *  @amupanelex
	 *  amu.sendCommand(CMD_SYSTEM_SLEEP);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 *  @note Device wakes on USB activity, I2C communication, or external interrupt; measurements are suspended during sleep.
	 *  @warning USB communication may be interrupted briefly.
	 */
	CMD_SYSTEM_SLEEP =						CMD_SYSTEM_OFFSET + 0x0F,
} CMD_SYSTEM_t;
#undef CMD_SYSTEM_OFFSET

/**
 * @brief Device Under Test (DUT) command identifiers
 * @ingroup cmd_dut
 * 
 * DUT commands configure and query parameters related to the device being tested.
 * These include physical characteristics, manufacturer information, and calibration data.
 */
typedef enum {
	/** @amutitle{DUT — Junction}
	 *  @amudesc{Sets or queries DUT junction type.}
	 *  @param junction Junction type ID (0=Unknown, 1=Silicon, 2=GaAs, 3=InGaP, 4=Ge)
	 *  @return Current junction type identifier
	 *
	 *  @amupanels
	 *  @amuscpi{DUT:JUNCtion[?]}
	 *  @amupanelex
	 *  DUT:JUNCtion 1
	 *  DUT:JUNCtion?
	 *  1
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_DUT_JUNCTION}
	 *  @amupanelex
	 *  uint8_t junction = amu.query<uint8_t>(CMD_DUT_JUNCTION);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 *  @note Junction type affects measurement range and calibration coefficients
	 */
	CMD_DUT_JUNCTION =						CMD_DUT_OFFSET + 0x00,
	
	/** @amutitle{DUT — Coverglass}
	 *  @amudesc{Sets or queries DUT coverglass type.}
	 *  @param coverglass Coverglass type ID (0=None, 1=0.1mm, 2=0.2mm, 3=0.3mm, 4=Custom)
	 *  @return Current coverglass type identifier
	 *
	 *  @amupanels
	 *  @amuscpi{DUT:COVERglass[?]}
	 *  @amupanelex
	 *  DUT:COVERglass 2
	 *  DUT:COVERglass?
	 *  2
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_DUT_COVERGLASS}
	 *  @amupanelex
	 *  uint8_t coverglass = amu.query<uint8_t>(CMD_DUT_COVERGLASS);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 *  @note Affects optical calibration and measurement corrections
	 */
	CMD_DUT_COVERGLASS =					CMD_DUT_OFFSET + 0x01,
	
	/** @amutitle{DUT — Interconnect}
	 *  @amudesc{Sets or queries DUT interconnect type.}
	 *  @param interconnect Interconnect type ID (0=Wire bonds, 1=Solder, 2=Conductive epoxy, 3=Other)
	 *  @return Current interconnect type identifier
	 *
	 *  @amupanels
	 *  @amuscpi{DUT:INTERconnect[?]}
	 *  @amupanelex
	 *  DUT:INTERconnect 2
	 *  DUT:INTERconnect?
	 *  2
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_DUT_INTERCONNECT}
	 *  @amupanelex
	 *  uint8_t ic = amu.query<uint8_t>(CMD_DUT_INTERCONNECT);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 *  @note Affects series resistance compensation in measurements
	 */
	CMD_DUT_INTERCONNECT =					CMD_DUT_OFFSET + 0x02,
	
	/** @amutitle{DUT — Reserved}
	 *  @amudesc{Reserved for future expansion.}
	 *
	 *  @amupanels
	 *  @amui2c{CMD_DUT_RESERVED}
	 *  @amupanelex
	 *  amu.sendCommand(CMD_DUT_RESERVED);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 *  @warning Do not use - reserved for future firmware versions
	 */
	CMD_DUT_RESERVED =						CMD_DUT_OFFSET + 0x03,
	
	/** @amutitle{DUT — Manufacturer}
	 *  @amudesc{Sets or queries DUT manufacturer name.}
	 *  @param manufacturer Manufacturer name string (max 32 characters)
	 *  @return Current manufacturer name
	 *
	 *  @amupanels
	 *  @amuscpi{DUT:MANufacturer[?]}
	 *  @amupanelex
	 *  DUT:MANufacturer?
	 *  Acme Solar
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_DUT_MANUFACTURER}
	 *  @amupanelex
	 *  char mfr[AMU_DUT_MANUFACTURER_STR_LEN]; amu.query<char>(CMD_DUT_MANUFACTURER, mfr, sizeof(mfr));
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 *  @note Used in automated test report generation
	 */
	CMD_DUT_MANUFACTURER =					CMD_DUT_OFFSET + 0x04,
	
	/** @amutitle{DUT — Model}
	 *  @amudesc{Sets or queries DUT model/part number.}
	 *  @param model Model/part number string (max 32 characters)
	 *  @return Current model designation
	 *
	 *  @amupanels
	 *  @amuscpi{DUT:MODel[?]}
	 *  @amupanelex
	 *  DUT:MODel?
	 *  AC-240
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_DUT_MODEL}
	 *  @amupanelex
	 *  char model[AMU_DUT_MODEL_STR_LEN]; amu.query<char>(CMD_DUT_MODEL, model, sizeof(model));
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 *  @note Essential for test data traceability and analysis
	 */
	CMD_DUT_MODEL =							CMD_DUT_OFFSET + 0x05,
	
	/** @amutitle{DUT — Technology}
	 *  @amudesc{Sets or queries DUT semiconductor technology.}
	 *  @param technology Technology string (e.g., "Silicon", "GaAs", "InGaP/GaAs/Ge", max 32 chars)
	 *  @return Current technology description
	 *
	 *  @amupanels
	 *  @amuscpi{DUT:TECHnology[?]}
	 *  @amupanelex
	 *  DUT:TECHnology?
	 *  IMM3J
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_DUT_TECHNOLOGY}
	 *  @amupanelex
	 *  char tech[AMU_DUT_TECHNOLOGY_STR_LEN]; amu.query<char>(CMD_DUT_TECHNOLOGY, tech, sizeof(tech));
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 *  @note Affects default measurement parameters and analysis algorithms
	 */
	CMD_DUT_TECHNOLOGY =					CMD_DUT_OFFSET + 0x06,
	
	/** @amutitle{DUT — Serial Number}
	 *  @amudesc{Sets or queries DUT serial number.}
	 *  @param serial Serial number string (max 32 characters)
	 *  @return Current DUT serial number
	 *
	 *  @amupanels
	 *  @amuscpi{DUT:SERialnumber[?]}
	 *  @amupanelex
	 *  DUT:SERialnumber "DUT-2023-001"
	 *  DUT:SERialnumber?
	 *  DUT-2023-001
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_DUT_SERIAL_NUMBER}
	 *  @amupanelex
	 *  char serial[AMU_DUT_SERIALNUM_STR_LEN]; amu.query<char>(CMD_DUT_SERIAL_NUMBER, serial, sizeof(serial));
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 *  @note Critical for individual device performance tracking
	 */
	CMD_DUT_SERIAL_NUMBER =					CMD_DUT_OFFSET + 0x07,
	
	/** @amutitle{DUT — Energy}
	 *  @amudesc{Sets or queries DUT radiation energy exposure.}
	 *  @param energy Total energy exposure in MeV (floating-point, 0.0 to 1e12)
	 *  @return Current cumulative energy exposure
	 *
	 *  @amupanels
	 *  @amuscpi{DUT:ENERGY[?]}
	 *  @amupanelex
	 *  DUT:ENERGY 1.5e15
	 *  DUT:ENERGY?
	 *  1.500000e+15
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_DUT_ENERGY}
	 *  @amupanelex
	 *  float energy = amu.query<float>(CMD_DUT_ENERGY);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 *  @note Essential for radiation effects analysis and modeling
	 */
	CMD_DUT_ENERGY =						CMD_DUT_OFFSET + 0x08,
	
	/** @amutitle{DUT — Dose}
	 *  @amudesc{Sets or queries DUT ionizing radiation dose.}
	 *  @param dose Total dose in krad(Si) (floating-point, 0.0 to 10000.0)
	 *  @return Current cumulative dose exposure
	 *
	 *  @amupanels
	 *  @amuscpi{DUT:DOSE[?]}
	 *  @amupanelex
	 *  DUT:DOSE 2.5e14
	 *  DUT:DOSE?
	 *  2.500000e+14
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_DUT_DOSE}
	 *  @amupanelex
	 *  float dose = amu.query<float>(CMD_DUT_DOSE);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 *  @note Used for TID degradation analysis and lifetime predictions
	 */
	CMD_DUT_DOSE =							CMD_DUT_OFFSET + 0x09,
	
	/** @amutitle{DUT — Notes}
	 *  @amudesc{Sets or queries DUT documentation notes.}
	 *  @param notes Text notes string (max 256 characters)
	 *  @return Current notes content
	 *
	 *  @amupanels
	 *  @amuscpi{DUT:NOTEs[?]}
	 *  @amupanelex
	 *  DUT:NOTEs "Post-irradiation test #3"
	 *  DUT:NOTEs?
	 *  Post-irradiation test #3
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_DUT_NOTES}
	 *  @amupanelex
	 *  char notes[AMU_NOTES_SIZE];
	 *  amu.query<char>(CMD_DUT_NOTES, notes, sizeof(notes));
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 *  @note Supports basic markdown formatting for rich documentation
	 */
	CMD_DUT_NOTES =							CMD_DUT_OFFSET + 0x0B,
	
	/** @amutitle{DUT — Temperature Sensor Type}
	 *  @amudesc{Sets or queries DUT temperature sensor type.}
	 *  @param type Sensor type ID (0=None, 1=Thermocouple, 2=RTD, 3=Thermistor, 4=Diode)
	 *  @return Current sensor type identifier
	 *
	 *  @amupanels
	 *  @amuscpi{DUT:TSENSor:TYPE[?]}
	 *  @amupanelex
	 *  DUT:TSENSor:TYPE 1
	 *  DUT:TSENSor:TYPE?
	 *  1
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_DUT_TSENSOR_TYPE}
	 *  @amupanelex
	 *  uint8_t type = amu.query<uint8_t>(CMD_DUT_TSENSOR_TYPE);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 *  @note Sensor type determines calibration coefficients and measurement range
	 */
	CMD_DUT_TSENSOR_TYPE =					CMD_DUT_OFFSET + 0x0D,
	
	/** @amutitle{DUT — Temperature Sensor Number}
	 *  @amudesc{Sets or queries number of DUT temperature sensors.}
	 *  @param count Number of sensors (range: 0-4)
	 *  @return Current number of configured sensors
	 *
	 *  @amupanels
	 *  @amuscpi{DUT:TSENSor:NUMber[?]}
	 *  @amupanelex
	 *  DUT:TSENSor:NUMber 4
	 *  DUT:TSENSor:NUMber?
	 *  4
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_DUT_TSENSOR_NUMBER}
	 *  @amupanelex
	 *  uint8_t count = amu.query<uint8_t>(CMD_DUT_TSENSOR_NUMBER);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 *  @note Maximum 4 sensors supported per DUT
	 */
	CMD_DUT_TSENSOR_NUMBER =				CMD_DUT_OFFSET + 0x0E,
	
	/** @amutitle{DUT — Temperature Sensor Fit}
	 *  @amudesc{Sets or queries temperature sensor calibration coefficients.}
	 *  @param coeffs Polynomial coefficients array [a0, a1, a2, a3] (T = a0 + a1*x + a2*x² + a3*x³)
	 *  @return Current calibration coefficients
	 *
	 *  @amupanels
	 *  @amuscpi{DUT:TSENSor:FIT[?]}
	 *  @amupanelex
	 *  DUT:TSENSor:FIT 1.0,0.5,0.01
	 *  DUT:TSENSor:FIT?
	 *  1.000000,0.500000,0.010000
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_DUT_TSENSOR_FIT}
	 *  @amupanelex
	 *  amu_coeff_t coeffs = amu.query<amu_coeff_t>(CMD_DUT_TSENSOR_FIT);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 *  @note Coefficients must be determined through calibration procedure
	 */
	CMD_DUT_TSENSOR_FIT =					CMD_DUT_OFFSET + 0x0F,
} CMD_DUT_t;
#undef CMD_DUT_OFFSET

/**
 * @brief Execution and measurement command identifiers
 * @ingroup cmd_exec
 * 
 * These commands trigger measurements and calibration procedures.
 * Most return measurement data or perform calibration operations.
 */
typedef enum {
	/** @amutitle{Measure — ADC Active}
	 *  @amudesc{Measures all active ADC channels}
	 *  @return Measurement data from all active channels
	 *
	 *  @amupanels
	 *  @amuscpi{MEASure:ADC:ACTive?}
	 *  @amuscpialt{MEASure:ADC:ACTive:RAW?}
	 *  @amupanelex
	 *  MEASure:ADC:ACTive?
	 *  1.234567
	 *  MEASure:ADC:ACTive:RAW?
	 *  52341
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_EXEC_MEAS_ACTIVE_CHANNELS}
	 *  @amupanelex
	 *  amu.sendCommand(CMD_EXEC_MEAS_ACTIVE_CHANNELS);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_EXEC_MEAS_ACTIVE_CHANNELS =			CMD_EXEC_OFFSET + 0x00,
	
	/** @amutitle{Measure — ADC Channel}
	 *  @amudesc{Measures specified ADC channel.}
	 *  @param channel ADC channel number (0-15)
	 *  @return Measurement data from specified channel
	 *
	 *  @amupanels
	 *  @amuscpi{MEASure:ADC:CH#[:RAW]?}
	 *  @amupanelex
	 *  MEASure:ADC:CH0?
	 *  2.567890
	 *  MEASure:ADC:CH3:RAW?
	 *  41256
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_MEAS_CH_VOLTAGE}
	 *  @amupanelex
	 *  float value = amu.measureChannel(3); // read ADC channel 3
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_EXEC_MEAS_CHANNEL =					CMD_EXEC_OFFSET + 0x01,
	
	/** @amutitle{Measure — ADC TSENSORS}
	 *  @amudesc{Measures all configured temperature sensors.}
	 *  @return Temperature data from all configured sensors
	 *
	 *  @amupanels
	 *  @amuscpi{MEASure:ADC:TSENSORS[:RAW]?}
	 *  @amupanelex
	 *  MEASure:ADC:TSENSORS?
	 *  25.3,26.1,24.8,25.5
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_EXEC_MEAS_TSENSORS}
	 *  @amupanelex
	 *  amu.sendCommand(CMD_EXEC_MEAS_TSENSORS);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_EXEC_MEAS_TSENSORS =				CMD_EXEC_OFFSET + 0x02,
	
	/** @amutitle{Measure — Internal Voltages}
	 *  @amudesc{Measures internal supply voltages.}
	 *  @return Internal supply voltage measurements
	 *
	 *  @amupanels
	 *  @amuscpi{MEASure:INTERNALvoltages?}
	 *  @amupanelex
	 *  MEASure:INTERNALvoltages?
	 *  3.30,5.02,12.01,-12.03
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_EXEC_MEAS_INTERNAL_VOLTAGES}
	 *  @amupanelex
	 *  amu_int_volt_t volts = amu.query<amu_int_volt_t>(CMD_EXEC_MEAS_INTERNAL_VOLTAGES);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_EXEC_MEAS_INTERNAL_VOLTAGES =		CMD_EXEC_OFFSET + 0x03,
	
	/** @amutitle{Measure — Sun Sensor}
	 *  @amudesc{Calculates sun sensor angles}
	 *  @return Sun sensor yaw and pitch angle measurements
	 *
	 *  @amupanels
	 *  @amuscpi{MEASure:SUNSensor?}
	 *  @amupanelex
	 *  MEASure:SUNSensor?
	 *  15.2,-8.7,0.856
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_EXEC_MEAS_SUN_SENSOR}
	 *  @amupanelex
	 *  quad_photo_sensor_t ss = amu.query<quad_photo_sensor_t>(CMD_EXEC_MEAS_SUN_SENSOR);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_EXEC_MEAS_SUN_SENSOR =				CMD_EXEC_OFFSET + 0x04,
	
	/** @amutitle{Measure — Pressure}
	 *  @amudesc{Measures pressure sensor}
	 *  @return Pressure sensor measurement
	 *
	 *  @amupanels
	 *  @amuscpi{MEASure:PRESSure?}
	 *  @amupanelex
	 *  MEASure:PRESSure?
	 *  1013.25
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_EXEC_MEAS_PRESSURE_SENSOR}
	 *  @amupanelex
	 *  press_data_t press = amu.query<press_data_t>(CMD_EXEC_MEAS_PRESSURE_SENSOR);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_EXEC_MEAS_PRESSURE_SENSOR =			CMD_EXEC_OFFSET + 0x05,
	
	/** @amutitle{ADC — Calibrate}
	 *  @amudesc{Initiates ADC calibration procedure.}
	 *  @param coeff ADC calibration coefficient to write (omit to query)
	 *  @return ADC calibration value
	 *
	 *  @amupanels
	 *  @amuscpi{ADC:CALibrate[?]}
	 *  @amupanelex
	 *  ADC:CALibrate
	 *  ADC:CALibrate?
	 *  COMPLETE
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_EXEC_ADC_CAL}
	 *  @amupanelex
	 *  amu.sendCommand(CMD_EXEC_ADC_CAL);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_EXEC_ADC_CAL =						CMD_EXEC_OFFSET + 0x07,
	
	/** @amutitle{ADC — Calibrate All Internal}
	 *  @amudesc{Calibrates all internal ADC references.}
	 *
	 *  @amupanels
	 *  @amuscpi{ADC:CALibrate:ALL:INTernal}
	 *  @amupanelex
	 *  ADC:CALibrate:ALL:INTernal
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_EXEC_ADC_CAL_ALL_INTERNAL}
	 *  @amupanelex
	 *  amu.sendCommand(CMD_EXEC_ADC_CAL_ALL_INTERNAL);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_EXEC_ADC_CAL_ALL_INTERNAL =			CMD_EXEC_OFFSET + 0x08,
	
	/** @amutitle{ADC — Save All Internal}
	 *  @amudesc{Saves internal ADC calibration data.}
	 *
	 *  @amupanels
	 *  @amuscpi{ADC:SAVE:ALL:INTernal}
	 *  @amupanelex
	 *  ADC:SAVE:ALL:INTernal
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_EXEC_ADC_CAL_SAVE_ALL_INTERNAL}
	 *  @amupanelex
	 *  amu.sendCommand(CMD_EXEC_ADC_CAL_SAVE_ALL_INTERNAL);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_EXEC_ADC_CAL_SAVE_ALL_INTERNAL =	CMD_EXEC_OFFSET + 0x09,
	
	/** @amutitle{DAC — Calibrate}
	 *  @amudesc{Initiates DAC calibration procedure.}
	 *  @param coeff DAC calibration coefficient to write (omit to query)
	 *  @return DAC calibration value
	 *
	 *  @amupanels
	 *  @amuscpi{DAC:CALibrate[?]}
	 *  @amupanelex
	 *  DAC:CALibrate
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_EXEC_DAC_CAL}
	 *  @amupanelex
	 *  amu.sendCommand(CMD_EXEC_DAC_CAL);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_EXEC_DAC_CAL =						CMD_EXEC_OFFSET + 0x0A,
	
	/** @amutitle{DAC — Calibrate Save}
	 *  @amudesc{Saves DAC calibration data.}
	 *
	 *  @amupanels
	 *  @amuscpi{DAC:CALibrate:SAVe}
	 *  @amupanelex
	 *  DAC:CALibrate
	 *  DAC:CALibrate:SAVe
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_EXEC_DAC_CAL_SAVE}
	 *  @amupanelex
	 *  amu.sendCommand(CMD_EXEC_DAC_CAL_SAVE);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_EXEC_DAC_CAL_SAVE =					CMD_EXEC_OFFSET + 0x0B,
	
	/** @amutitle{ADC — Calibrate Temperature Sensor}
	 *  @amudesc{Calibrates temperature sensor at 25°C.}
	 *
	 *  @amupanels
	 *  @amuscpi{ADC:CALibrate:TSENSor}
	 *  @amupanelex
	 *  ADC:CALibrate:TSENSor
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_EXEC_TSENSOR_CAL_25C}
	 *  @amupanelex
	 *  amu.sendCommand(CMD_EXEC_TSENSOR_CAL_25C);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_EXEC_TSENSOR_CAL_25C =				CMD_EXEC_OFFSET + 0x0C,
	
	/** @amutitle{DUT — Temperature Sensor Fit Save}
	 *  @amudesc{Saves temperature sensor calibration coefficients.}
	 *
	 *  @amupanels
	 *  @amuscpi{DUT:TSENSor:FIT:SAVE}
	 *  @amupanelex
	 *  DUT:TSENSor:FIT:SAVE
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_EXEC_TSENSOR_COEFF_SAVE}
	 *  @amupanelex
	 *  amu.sendCommand(CMD_EXEC_TSENSOR_COEFF_SAVE);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_EXEC_TSENSOR_COEFF_SAVE =			CMD_EXEC_OFFSET + 0x0D,
	
	/** @amutitle{Sun Sensor — Fit Save}
	 *  @amudesc{Saves sun sensor calibration coefficients.}
	 *
	 *  @amupanels
	 *  @amuscpi{SUNSensor:FIT:SAVE}
	 *  @amupanelex
	 *  SUNSensor:FIT:SAVE
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_EXEC_SUNSENSOR_COEFF_SAVE}
	 *  @amupanelex
	 *  amu.sendCommand(CMD_EXEC_SUNSENSOR_COEFF_SAVE);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_EXEC_SUNSENSOR_COEFF_SAVE =			CMD_EXEC_OFFSET + 0x0E,
	
	/** @amutitle{Heater — PID Save}
	 *  @amudesc{Saves heater PID controller parameters.}
	 *
	 *  @amupanels
	 *  @amuscpi{HEATer:PID:SAVE}
	 *  @amupanelex
	 *  HEATer:PID:SAVE
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_EXEC_HEATER_PID_SAVE}
	 *  @amupanelex
	 *  amu.sendCommand(CMD_EXEC_HEATER_PID_SAVE);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_EXEC_HEATER_PID_SAVE  =				CMD_EXEC_OFFSET + 0x0F,
} CMD_EXEC_t;
#undef CMD_EXEC_OFFSET

/**
 * @brief I-V sweep control command identifiers
 * @ingroup cmd_sweep
 * 
 * These commands control current-voltage sweep operations for solar cell characterization.
 * Sweeps can be configured, triggered, and data can be stored/retrieved from EEPROM.
 */
typedef enum {
	/** @amutitle{Sweep — Disable}
	 *  @amudesc{Disables sweep functionality.}
	 *
	 *  @amupanels
	 *  @amuscpi{SWEEP:DISable}
	 *  @amupanelex
	 *  SWEEP:DISable
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_SWEEP_DISABLE}
	 *  @amupanelex
	 *  amu.sendCommand(CMD_SWEEP_DISABLE);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_SWEEP_DISABLE =						CMD_SWEEP_OFFSET + 0x00,
	
	/** @amutitle{Sweep — Enable}
	 *  @amudesc{Enables sweep functionality.}
	 *
	 *  @amupanels
	 *  @amuscpi{SWEEP:ENAble}
	 *  @amupanelex
	 *  SWEEP:ENAble
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_SWEEP_ENABLE}
	 *  @amupanelex
	 *  amu.sendCommand(CMD_SWEEP_ENABLE);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_SWEEP_ENABLE =						CMD_SWEEP_OFFSET + 0x01,
	
	/** @amutitle{Sweep — Trigger}
	 *  @amudesc{Initiates complete I-V sweep measurement.}
	 *
	 *  @amupanels
	 *  @amuscpi{SWEEP:TRIGger}
	 *  @amupanelex
	 *  SWEEP:ENAble
	 *  SWEEP:TRIGger
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_SWEEP_TRIG_SWEEP}
	 *  @amupanelex
	 *  amu.sendCommand(CMD_SWEEP_TRIG_SWEEP);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_SWEEP_TRIG_SWEEP =					CMD_SWEEP_OFFSET + 0x02,
	
	/** @amutitle{Sweep — Trigger ISC}
	 *  @amudesc{Measures short-circuit current.}
	 *  @return Short-circuit current (Isc) value
	 *
	 *  @amupanels
	 *  @amuscpi{SWEEP:TRIGger:ISC?}
	 *  @amupanelex
	 *  SWEEP:TRIGger:ISC?
	 *  0.00234567
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_SWEEP_TRIG_ISC}
	 *  @amupanelex
	 *  amu.sendCommand(CMD_SWEEP_TRIG_ISC);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_SWEEP_TRIG_ISC =					CMD_SWEEP_OFFSET + 0x03,
	
	/** @amutitle{Sweep — Trigger VOC}
	 *  @amudesc{Measures open-circuit voltage.}
	 *  @return Open-circuit voltage (Voc) value
	 *
	 *  @amupanels
	 *  @amuscpi{SWEEP:TRIGger:VOC?}
	 *  @amupanelex
	 *  SWEEP:TRIGger:VOC?
	 *  0.567890
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_SWEEP_TRIG_VOC}
	 *  @amupanelex
	 *  amu.sendCommand(CMD_SWEEP_TRIG_VOC);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_SWEEP_TRIG_VOC =					CMD_SWEEP_OFFSET + 0x04,
	
	/** @amutitle{Sweep — Config Save}
	 *  @amudesc{Saves sweep configuration.}
	 *
	 *  @amupanels
	 *  @amuscpi{SWEEP:CONFig:SAVe}
	 *  @amupanelex
	 *  SWEEP:CONFig:SAVe
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_SWEEP_CONFIG_SAVE}
	 *  @amupanelex
	 *  amu.sendCommand(CMD_SWEEP_CONFIG_SAVE);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_SWEEP_CONFIG_SAVE =					CMD_SWEEP_OFFSET + 0x05,
	
	/** @amutitle{Sweep — EEPROM Save}
	 *  @amudesc{Saves sweep data to EEPROM.}
	 *
	 *  @amupanels
	 *  @amuscpi{SWEEP:EEPROM:SAVE}
	 *  @amupanelex
	 *  SWEEP:EEPROM:SAVE
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_SWEEP_IV_SAVE_TO_EEPROM}
	 *  @amupanelex
	 *  amu.sendCommand(CMD_SWEEP_IV_SAVE_TO_EEPROM);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_SWEEP_IV_SAVE_TO_EEPROM =			CMD_SWEEP_OFFSET + 0x06,
	
	/** @amutitle{Sweep — EEPROM LOAD}
	 *  @amudesc{Loads sweep data from EEPROM.}
	 *  @return Previously saved sweep measurement data
	 *
	 *  @amupanels
	 *  @amuscpi{SWEEP:EEPROM:LOAD}
	 *  @amupanelex
	 *  SWEEP:EEPROM:LOAD
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_SWEEP_IV_LOAD_FROM_EEPROM}
	 *  @amupanelex
	 *  amu.sendCommand(CMD_SWEEP_IV_LOAD_FROM_EEPROM);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_SWEEP_IV_LOAD_FROM_EEPROM =			CMD_SWEEP_OFFSET + 0x07,
	
	/** @amutitle{Sweep — AES Encode}
	 *  @amudesc{Encrypts sweep data using AES.}
	 *
	 *  @amupanels
	 *  @amuscpi{SWEEP:AES:ENCode}
	 *  @amupanelex
	 *  SWEEP:AES:ENCode
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_SWEEP_AES_ENCODE}
	 *  @amupanelex
	 *  amu.sendCommand(CMD_SWEEP_AES_ENCODE);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_SWEEP_AES_ENCODE =					CMD_SWEEP_OFFSET + 0x08,
	
	/** @amutitle{Sweep — AES Decode}
	 *  @amudesc{Decrypts AES-encrypted sweep data.}
	 *
	 *  @amupanels
	 *  @amuscpi{SWEEP:AES:DECode}
	 *  @amupanelex
	 *  SWEEP:AES:DECode
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_SWEEP_AES_DECODE}
	 *  @amupanelex
	 *  amu.sendCommand(CMD_SWEEP_AES_DECODE);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_SWEEP_AES_DECODE =					CMD_SWEEP_OFFSET + 0x09,
	
	/** @amutitle{Sweep — Datapoint Save}
	 *  @amudesc{Saves single voltage/current data point.}
	 *
	 *  @amupanels
	 *  @amuscpi{SWEEP:DATApoint:SAVE}
	 *  @amupanelex
	 *  SWEEP:DATApoint:SAVE
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_SWEEP_DATAPOINT_SAVE}
	 *  @amupanelex
	 *  amu.sendCommand(CMD_SWEEP_DATAPOINT_SAVE);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_SWEEP_DATAPOINT_SAVE =				CMD_SWEEP_OFFSET + 0x0A,
	
	/** @amutitle{Sweep — Trigger Initialize}
	 *  @amudesc{Initializes sweep trigger system.}
	 *
	 *  @amupanels
	 *  @amuscpi{SWEEP:TRIGger:INITialize}
	 *  @amupanelex
	 *  SWEEP:TRIGger:INITialize
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_SWEEP_TRIG_INIT}
	 *  @amupanelex
	 *  amu.sendCommand(CMD_SWEEP_TRIG_INIT);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_SWEEP_TRIG_INIT =					CMD_SWEEP_OFFSET + 0x0B,
	
	/** @amutitle{Sweep — Datapoint LOAD}
	 *  @amudesc{Loads single voltage/current data point.}
	 *  @return Single voltage/current data point
	 *
	 *  @amupanels
	 *  @amuscpi{SWEEP:DATApoint:LOAD}
	 *  @amupanelex
	 *  SWEEP:DATApoint:LOAD
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_SWEEP_DATAPOINT_LOAD}
	 *  @amupanelex
	 *  amu.sendCommand(CMD_SWEEP_DATAPOINT_LOAD);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_SWEEP_DATAPOINT_LOAD =				CMD_SWEEP_OFFSET + 0x0C,
} CMD_SWEEP_t;
#undef CMD_SWEEP_OFFSET

/**
 * @brief Auxiliary subsystem command identifiers
 * @ingroup cmd_aux
 * 
 * These commands control auxiliary subsystems including DAC output, heater control,
 * and sun sensor calibration. Commands support both raw values and calibrated units.
 */
typedef enum {	
	/** @amutitle{DAC — State}
	 *  @amudesc{Sets or queries DAC output enable state.}
	 *  @param state DAC enable state (0=disabled, 1=enabled)
	 *  @return Current DAC enable state
	 *
	 *  @amupanels
	 *  @amuscpi{DAC:STATE[?]}
	 *  @amupanelex
	 *  DAC:STATE 1
	 *  DAC:STATE?
	 *  1
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_AUX_DAC_STATE}
	 *  @amupanelex
	 *  uint8_t val = amu.query<uint8_t>(CMD_AUX_DAC_STATE);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_AUX_DAC_STATE =						CMD_AUX_OFFSET + 0x00,
	
	/** @amutitle{DAC — Current}
	 *  @amudesc{Sets or queries DAC current output.}
	 *  @param current Current output in amperes
	 *  @return Current DAC current setting in amperes
	 *
	 *  @amupanels
	 *  @amuscpi{DAC:CURRent[?]}
	 *  @amupanelex
	 *  DAC:CURRent 0.5
	 *  DAC:CURRent?
	 *  0.500000
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_AUX_DAC_CURRENT}
	 *  @amupanelex
	 *  float val = amu.query<float>(CMD_AUX_DAC_CURRENT);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_AUX_DAC_CURRENT =					CMD_AUX_OFFSET + 0x01,
	
	/** @amutitle{DAC — Current Raw}
	 *  @amudesc{Sets or queries DAC current output (raw).}
	 *  @param raw_value Raw 16-bit DAC value
	 *  @return Current raw DAC value
	 *
	 *  @amupanels
	 *  @amuscpi{DAC:CURRent:RAW[?]}
	 *  @amupanelex
	 *  DAC:CURRent:RAW 2048
	 *  DAC:CURRent:RAW?
	 *  2048
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_AUX_DAC_CURRENT_RAW}
	 *  @amupanelex
	 *  uint16_t val = amu.query<uint16_t>(CMD_AUX_DAC_CURRENT_RAW);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_AUX_DAC_CURRENT_RAW =				CMD_AUX_OFFSET + 0x02,
	
	/** @amutitle{DAC — Voltage}
	 *  @amudesc{Sets or queries DAC voltage output.}
	 *  @param voltage Voltage output in volts
	 *  @return Current DAC voltage setting in volts
	 *
	 *  @amupanels
	 *  @amuscpi{DAC:VOLTage[?]}
	 *  @amupanelex
	 *  DAC:VOLTage 2.5
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_AUX_DAC_VOLTAGE}
	 *  @amupanelex
	 *  float voltage = amu.query<float>(CMD_AUX_DAC_VOLTAGE);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_AUX_DAC_VOLTAGE =					CMD_AUX_OFFSET + 0x03,
	
	/** @amutitle{DAC — Voltage Raw}
	 *  @amudesc{Sets or queries DAC voltage output (raw).}
	 *  @param raw_value Raw 16-bit DAC value
	 *  @return Current raw DAC voltage value
	 *
	 *  @amupanels
	 *  @amuscpi{DAC:VOLTage:RAW[?]}
	 *  @amupanelex
	 *  DAC:VOLTage:RAW 2048
	 *  DAC:VOLTage:RAW?
	 *  2048
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_AUX_DAC_VOLTAGE_RAW}
	 *  @amupanelex
	 *  uint16_t val = amu.query<uint16_t>(CMD_AUX_DAC_VOLTAGE_RAW);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_AUX_DAC_VOLTAGE_RAW =				CMD_AUX_OFFSET + 0x04,
	
	/** @amutitle{DAC — Offset}
	 *  @amudesc{Sets or queries DAC offset calibration.}
	 *  @param offset DAC offset calibration value
	 *  @return Current DAC offset value
	 *
	 *  @amupanels
	 *  @amuscpi{DAC:OFFset[?]}
	 *  @amupanelex
	 *  DAC:OFFset 2048
	 *  DAC:OFFset?
	 *  2048
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_AUX_DAC_OFFSET}
	 *  @amupanelex
	 *  uint16_t val = amu.query<uint16_t>(CMD_AUX_DAC_OFFSET);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_AUX_DAC_OFFSET =					CMD_AUX_OFFSET + 0x05,
	
	/** @amutitle{DAC — Offset Correction}
	 *  @amudesc{Sets or queries DAC offset correction coefficient.}
	 *  @param correction DAC offset correction coefficient
	 *  @return Current DAC offset correction coefficient
	 *
	 *  @amupanels
	 *  @amuscpi{DAC:OFFset:CORRection[?]}
	 *  @amupanelex
	 *  DAC:OFFset:CORRection 0.0
	 *  DAC:OFFset:CORRection?
	 *  0.000000
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_AUX_DAC_OFFSET_CORRECTION}
	 *  @amupanelex
	 *  float val = amu.query<float>(CMD_AUX_DAC_OFFSET_CORRECTION);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_AUX_DAC_OFFSET_CORRECTION =			CMD_AUX_OFFSET + 0x06,
	
	/** @amutitle{DAC — Gain Correction}
	 *  @amudesc{Sets or queries DAC gain correction coefficient.}
	 *  @param correction DAC gain correction coefficient
	 *  @return Current DAC gain correction coefficient
	 *
	 *  @amupanels
	 *  @amuscpi{DAC:GAIN:CORRection[?]}
	 *  @amupanelex
	 *  DAC:GAIN:CORRection 1.0
	 *  DAC:GAIN:CORRection?
	 *  1.000000
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_AUX_DAC_GAIN_CORRECTION}
	 *  @amupanelex
	 *  float val = amu.query<float>(CMD_AUX_DAC_GAIN_CORRECTION);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_AUX_DAC_GAIN_CORRECTION =			CMD_AUX_OFFSET + 0x07,
	
	/** @amutitle{Heater — State}
	 *  @amudesc{Sets or queries heater enable state.}
	 *  @param state Heater enable state (0=off, 1=on)
	 *  @return Current heater enable state
	 *
	 *  @amupanels
	 *  @amuscpi{HEATer:STATE[?]}
	 *  @amupanelex
	 *  HEATer:STATE 1
	 *  HEATer:STATE?
	 *  1
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_AUX_HEATER_STATE}
	 *  @amupanelex
	 *  uint8_t val = amu.query<uint8_t>(CMD_AUX_HEATER_STATE);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_AUX_HEATER_STATE =					CMD_AUX_OFFSET + 0x08,
	
	/** @amutitle{Heater — Setpoint}
	 *  @amudesc{Sets or queries heater temperature setpoint.}
	 *  @param setpoint Temperature setpoint in Celsius
	 *  @return Current heater temperature setpoint
	 *
	 *  @amupanels
	 *  @amuscpi{HEATer:SETpoint[?]}
	 *  @amupanelex
	 *  HEATer:SETpoint 25.0
	 *  HEATer:SETpoint?
	 *  25.000000
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_AUX_HEATER_SETPOINT}
	 *  @amupanelex
	 *  float val = amu.query<float>(CMD_AUX_HEATER_SETPOINT);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_AUX_HEATER_SETPOINT =				CMD_AUX_OFFSET + 0x09,
	
	/** @amutitle{Heater — PID}
	 *  @amudesc{Sets or queries heater PID controller coefficients.}
	 *  @param coeffs PID controller coefficients [Kp, Ki, Kd]
	 *  @return Current PID controller coefficients
	 *
	 *  @amupanels
	 *  @amuscpi{HEATer:PID[?]}
	 *  @amupanelex
	 *  HEATer:PID 1.0,0.1,0.01
	 *  HEATer:PID?
	 *  1.000000,0.100000,0.010000
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_AUX_HEATER_PID}
	 *  @amupanelex
	 *  amu_pid_t pid = amu.query<amu_pid_t>(CMD_AUX_HEATER_PID);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_AUX_HEATER_PID =					CMD_AUX_OFFSET + 0x0A,
	
	/** @amutitle{Sun Sensor — Fit YAW}
	 *  @amudesc{Sets or queries sun sensor yaw calibration coefficients.}
	 *  @param coeffs Yaw angle calibration polynomial coefficients
	 *  @return Current yaw angle calibration coefficients
	 *
	 *  @amupanels
	 *  @amuscpi{SUNSensor:FIT:YAW[?]}
	 *  @amupanelex
	 *  SUNSensor:FIT:YAW 1.0,0.0,0.0
	 *  SUNSensor:FIT:YAW?
	 *  1.000000,0.000000,0.000000
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_AUX_SUNSENSOR_FIT_YAW_COEFF}
	 *  @amupanelex
	 *  amu_coeff_t coeff = amu.query<amu_coeff_t>(CMD_AUX_SUNSENSOR_FIT_YAW_COEFF);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_AUX_SUNSENSOR_FIT_YAW_COEFF =		CMD_AUX_OFFSET + 0x0B,
	
	/** @amutitle{Sun Sensor — Fit PITCH}
	 *  @amudesc{Sets or queries sun sensor pitch calibration coefficients.}
	 *  @param coeffs Pitch angle calibration polynomial coefficients
	 *  @return Current pitch angle calibration coefficients
	 *
	 *  @amupanels
	 *  @amuscpi{SUNSensor:FIT:PITCH[?]}
	 *  @amupanelex
	 *  SUNSensor:FIT:PITCH 1.0,0.0,0.0
	 *  SUNSensor:FIT:PITCH?
	 *  1.000000,0.000000,0.000000
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_AUX_SUNSENSOR_FIT_PITCH_COEFF}
	 *  @amupanelex
	 *  amu_coeff_t coeff = amu.query<amu_coeff_t>(CMD_AUX_SUNSENSOR_FIT_PITCH_COEFF);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_AUX_SUNSENSOR_FIT_PITCH_COEFF =		CMD_AUX_OFFSET + 0x0C,
	
	/** @amutitle{Sun Sensor — HVAL}
	 *  @amudesc{Sets or queries sun sensor horizontal reference.}
	 *  @param hval Horizontal reference calibration value
	 *  @return Current horizontal reference value
	 *
	 *  @amupanels
	 *  @amuscpi{SUNSensor:HVAL[?]}
	 *  @amupanelex
	 *  SUNSensor:HVAL 1.0
	 *  SUNSensor:HVAL?
	 *  1.000000
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_AUX_SUNSENSOR_HVAL}
	 *  @amupanelex
	 *  float val = amu.query<float>(CMD_AUX_SUNSENSOR_HVAL);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_AUX_SUNSENSOR_HVAL =				CMD_AUX_OFFSET + 0x0D,
	
	/** @amutitle{Sun Sensor — RVAL}
	 *  @amudesc{Sets or queries sun sensor radial reference.}
	 *  @param rval Radial reference calibration value
	 *  @return Current radial reference value
	 *
	 *  @amupanels
	 *  @amuscpi{SUNSensor:RVAL[?]}
	 *  @amupanelex
	 *  SUNSensor:RVAL 1.0
	 *  SUNSensor:RVAL?
	 *  1.000000
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_AUX_SUNSENSOR_RVAL}
	 *  @amupanelex
	 *  float val = amu.query<float>(CMD_AUX_SUNSENSOR_RVAL);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_AUX_SUNSENSOR_RVAL =				CMD_AUX_OFFSET + 0x0E,
	
	/** @amutitle{Sun Sensor — Threshold}
	 *  @amudesc{Sets or queries sun sensor detection threshold.}
	 *  @param threshold Sun sensor detection threshold value
	 *  @return Current detection threshold value
	 *
	 *  @amupanels
	 *  @amuscpi{SUNSensor:THRESHold[?]}
	 *  @amupanelex
	 *  SUNSensor:THRESHold 0.1
	 *  SUNSensor:THRESHold?
	 *  0.100000
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_AUX_SUNSENSOR_THRESHOLD}
	 *  @amupanelex
	 *  float val = amu.query<float>(CMD_AUX_SUNSENSOR_THRESHOLD);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_AUX_SUNSENSOR_THRESHOLD =			CMD_AUX_OFFSET + 0x0F,	
} CMD_AUX_t;
#undef CMD_AUX_OFFSET

/**
 * @brief ADC channel configuration command identifiers
 * @ingroup cmd_adc
 * 
 * These commands configure individual ADC channels including setup, filtering,
 * gain settings, and calibration procedures. Channel numbers range from 0-15.
 */
typedef enum {
	/** @amutitle{ADC — Channel}
	 *  @amudesc{Sets or queries ADC channel register.}
	 *  @param channel Channel number (0-15)
	 *  @param value Register configuration value
	 *  @return Current ADC channel register value
	 *
	 *  @amupanels
	 *  @amuscpi{ADC:CH#[?]}
	 *  @amupanelex
	 *  ADC:CH0 2048
	 *  ADC:CH0?
	 *  2048
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_ADC_CH_REG}
	 *  @amupanelex
	 *  uint16_t val = amu.query<uint16_t>(CMD_ADC_CH_REG);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_ADC_CH_REG =						CMD_ADC_CH_OFFSET + 0x00,
	
	/** @amutitle{ADC — Channel Setup}
	 *  @amudesc{Sets or queries ADC channel setup.}
	 *  @param channel Channel number (0-15)
	 *  @param setup Setup register configuration
	 *  @return Current ADC channel setup configuration
	 *
	 *  @amupanels
	 *  @amuscpi{ADC:CH#:SETup[?]}
	 *  @amupanelex
	 *  ADC:CH0:SETup 2048
	 *  ADC:CH0:SETup?
	 *  2048
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_ADC_CH_SETUP}
	 *  @amupanelex
	 *  uint16_t val = amu.query<uint16_t>(CMD_ADC_CH_SETUP);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_ADC_CH_SETUP =						CMD_ADC_CH_OFFSET + 0x01,
	
	/** @amutitle{ADC — Channel Filter}
	 *  @amudesc{Sets or queries ADC channel filter.}
	 *  @param channel Channel number (0-15)
	 *  @param filter Digital filter configuration
	 *  @return Current ADC channel filter settings
	 *
	 *  @amupanels
	 *  @amuscpi{ADC:CH#:FILTer[?]}
	 *  @amupanelex
	 *  ADC:CH0:FILTer 8388608
	 *  ADC:CH0:FILTer?
	 *  8388608
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_ADC_CH_FILTER}
	 *  @amupanelex
	 *  uint32_t val = amu.query<uint32_t>(CMD_ADC_CH_FILTER);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_ADC_CH_FILTER =						CMD_ADC_CH_OFFSET + 0x02,
	
	/** @amutitle{ADC — Channel Rate}
	 *  @amudesc{Sets or queries ADC channel sample rate.}
	 *  @param channel Channel number (0-15)
	 *  @param rate Sample rate in Hz
	 *  @return Current ADC channel sample rate
	 *
	 *  @amupanels
	 *  @amuscpi{ADC:CH#:RATE[?]}
	 *  @amupanelex
	 *  ADC:CH0:RATE 100.0
	 *  ADC:CH0:RATE?
	 *  100.000000
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_ADC_CH_RATE}
	 *  @amupanelex
	 *  float val = amu.query<float>(CMD_ADC_CH_RATE);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_ADC_CH_RATE =						CMD_ADC_CH_OFFSET + 0x03,
	
	/** @amutitle{ADC — Channel PGA}
	 *  @amudesc{Sets or queries ADC channel PGA setting.}
	 *  @param channel Channel number (0-15)
	 *  @param gain PGA gain setting (1, 2, 4, 8, 16, 32, 64, 128)
	 *  @return Current ADC channel PGA setting
	 *
	 *  @amupanels
	 *  @amuscpi{ADC:CH#:PGA[?]}
	 *  @amupanelex
	 *  ADC:CH0:PGA 4
	 *  ADC:CH0:PGA?
	 *  4
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_ADC_CH_PGA}
	 *  @amupanelex
	 *  uint8_t val = amu.query<uint8_t>(CMD_ADC_CH_PGA);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_ADC_CH_PGA =						CMD_ADC_CH_OFFSET + 0x04,
	
	/** @amutitle{ADC — Channel Max}
	 *  @amudesc{Queries maximum input range for current PGA.}
	 *  @param channel Channel number (0-15)
	 *  @return Maximum input range for current PGA setting
	 *
	 *  @amupanels
	 *  @amuscpi{ADC:CH#:MAX?}
	 *  @amupanelex
	 *  ADC:CH0:MAX?
	 *  2.500000
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_ADC_CH_PGA_MAX}
	 *  @amupanelex
	 *  float val = amu.query<float>(CMD_ADC_CH_PGA_MAX);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_ADC_CH_PGA_MAX =					CMD_ADC_CH_OFFSET + 0x05,
	
	/** @amutitle{ADC — Voltage Max PGA}
	 *  @amudesc{Queries maximum voltage range for PGA.}
	 *  @param pga PGA gain setting
	 *  @return Maximum voltage range for specified PGA setting
	 *
	 *  @amupanels
	 *  @amuscpi{ADC:VOLTage:MAX:PGA#?}
	 *  @amupanelex
	 *  ADC:VOLTage:MAX:PGA0 2.5
	 *  ADC:VOLTage:MAX:PGA0?
	 *  2.500000
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_ADC_CH_PGA_VMAX}
	 *  @amupanelex
	 *  float val = amu.query<float>(CMD_ADC_CH_PGA_VMAX);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_ADC_CH_PGA_VMAX =					CMD_ADC_CH_OFFSET + 0x06,
	
	/** @amutitle{ADC — Current Max PGA}
	 *  @amudesc{Queries maximum current range for PGA.}
	 *  @param pga PGA gain setting
	 *  @return Maximum current range for specified PGA setting
	 *
	 *  @amupanels
	 *  @amuscpi{ADC:CURRent:MAX:PGA#?}
	 *  @amupanelex
	 *  ADC:CURRent:MAX:PGA0 2.5
	 *  ADC:CURRent:MAX:PGA0?
	 *  2.500000
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_ADC_CH_PGA_IMAX}
	 *  @amupanelex
	 *  float val = amu.query<float>(CMD_ADC_CH_PGA_IMAX);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_ADC_CH_PGA_IMAX =					CMD_ADC_CH_OFFSET + 0x07,
	
	/** @amutitle{ADC — Channel Save}
	 *  @amudesc{Saves ADC channel configuration.}
	 *  @param channel Channel number (0-15)
	 *
	 *  @amupanels
	 *  @amuscpi{ADC:CH#:SAVE}
	 *  @amupanelex
	 *  ADC:CH0:SAVE
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_ADC_CH_SAVE}
	 *  @amupanelex
	 *  amu.sendCommand(CMD_ADC_CH_SAVE);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_ADC_CH_SAVE =						CMD_ADC_CH_OFFSET + 0x08,
	
	/** @amutitle{ADC — Channel Offset}
	 *  @amudesc{Sets or queries ADC channel offset coefficient.}
	 *  @param channel Channel number (0-15)
	 *  @param coefficient Offset calibration coefficient
	 *  @return Current offset calibration coefficient (int32_t format)
	 *
	 *  @amupanels
	 *  @amuscpi{ADC:CH#:OFFset[?]}
	 *  @amupanelex
	 *  ADC:CH0:OFFset -512
	 *  ADC:CH0:OFFset?
	 *  -512
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_ADC_CH_OFFSET_COEFF}
	 *  @amupanelex
	 *  int32_t val = amu.query<int32_t>(CMD_ADC_CH_OFFSET_COEFF);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_ADC_CH_OFFSET_COEFF =				CMD_ADC_CH_OFFSET + 0x09,
	
	/** @amutitle{ADC — Channel Gain}
	 *  @amudesc{Sets or queries ADC channel gain coefficient.}
	 *  @param channel Channel number (0-15)
	 *  @param coefficient Gain calibration coefficient
	 *  @return Current gain calibration coefficient (uint32_t format)
	 *
	 *  @amupanels
	 *  @amuscpi{ADC:CH#:GAIN[?]}
	 *  @amupanelex
	 *  ADC:CH0:GAIN 8388608
	 *  ADC:CH0:GAIN?
	 *  8388608
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_ADC_CH_GAIN_COEFF}
	 *  @amupanelex
	 *  uint32_t val = amu.query<uint32_t>(CMD_ADC_CH_GAIN_COEFF);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_ADC_CH_GAIN_COEFF =					CMD_ADC_CH_OFFSET + 0x0A,
	
	/** @amutitle{ADC — Channel Calibrate Internal}
	 *  @amudesc{Performs internal ADC channel calibration.}
	 *  @param channel Channel number (0-15)
	 *
	 *  @amupanels
	 *  @amuscpi{ADC:CH#:CALibrate:INTernal}
	 *  @amupanelex
	 *  ADC:CH0:CALibrate:INTernal
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_ADC_CH_CAL_INTERNAL}
	 *  @amupanelex
	 *  amu.sendCommand(CMD_ADC_CH_CAL_INTERNAL);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_ADC_CH_CAL_INTERNAL =				CMD_ADC_CH_OFFSET + 0x0B,
	
	/** @amutitle{ADC — Channel Calibrate Zero}
	 *  @amudesc{Performs zero-scale ADC calibration.}
	 *  @param channel Channel number (0-15)
	 *
	 *  @amupanels
	 *  @amuscpi{ADC:CH#:CALibrate:ZERO}
	 *  @amupanelex
	 *  ADC:CH0:CALibrate:ZERO
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_ADC_CH_CAL_ZERO_SCALE}
	 *  @amupanelex
	 *  amu.sendCommand(CMD_ADC_CH_CAL_ZERO_SCALE);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_ADC_CH_CAL_ZERO_SCALE =				CMD_ADC_CH_OFFSET + 0x0C,
	
	/** @amutitle{ADC — Channel Calibrate Full}
	 *  @amudesc{Performs full-scale ADC calibration.}
	 *  @param channel Channel number (0-15)
	 *
	 *  @amupanels
	 *  @amuscpi{ADC:CH#:CALibrate:FULL}
	 *  @amupanelex
	 *  ADC:CH0:CALibrate:FULL
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_ADC_CH_CAL_FULL_SCALE}
	 *  @amupanelex
	 *  amu.sendCommand(CMD_ADC_CH_CAL_FULL_SCALE);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_ADC_CH_CAL_FULL_SCALE =				CMD_ADC_CH_OFFSET + 0x0D,
	
	/** @amutitle{ADC — Channel Calibrate Reset}
	 *  @amudesc{Resets ADC channel calibration.}
	 *  @param channel Channel number (0-15)
	 *
	 *  @amupanels
	 *  @amuscpi{ADC:CH#:CALibrate:RESet}
	 *  @amupanelex
	 *  ADC:CH0:CALibrate:RESet
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_ADC_CH_CAL_RESET}
	 *  @amupanelex
	 *  amu.sendCommand(CMD_ADC_CH_CAL_RESET);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_ADC_CH_CAL_RESET =					CMD_ADC_CH_OFFSET + 0x0E,
	
	/** @amutitle{ADC — Channel Calibrate Save}
	 *  @amudesc{Saves ADC channel calibration.}
	 *  @param channel Channel number (0-15)
	 *
	 *  @amupanels
	 *  @amuscpi{ADC:CH#:CALibrate:SAVe}
	 *  @amupanelex
	 *  ADC:CH0:CALibrate:SAVe
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_ADC_CH_CAL_SAVE}
	 *  @amupanelex
	 *  amu.sendCommand(CMD_ADC_CH_CAL_SAVE);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_ADC_CH_CAL_SAVE =					CMD_ADC_CH_OFFSET + 0x0F,
} CMD_ADC_CH_t;
#undef CMD_ADC_CH_OFFSET

/**
 * @brief Measurement channel command identifiers
 * @ingroup cmd_meas
 * 
 * These commands perform measurements on specific ADC channels.
 * Channel assignments are based on AMU_ADC_CH_t definitions.
 */
typedef enum {
	/** @amutitle{Measure — ADC Voltage}
	 *  @amudesc{Measures dedicated voltage channel.}
	 *  @return Voltage measurement from dedicated voltage channel
	 *
	 *  @amupanels
	 *  @amuscpi{MEASure:ADC:VOLTage[:RAW]?}
	 *  @amupanelex
	 *  MEASure:ADC:VOLTage?
	 *  2.305000
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_MEAS_CH_VOLTAGE}
	 *  @amupanelex
	 *  float val = amu.query<float>(CMD_MEAS_CH_VOLTAGE);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_MEAS_CH_VOLTAGE =					CMD_MEAS_CH_CMD_OFFSET + 0x00,
	
	/** @amutitle{Measure — ADC Current}
	 *  @amudesc{Measures dedicated current channel.}
	 *  @return Current measurement from dedicated current channel
	 *
	 *  @amupanels
	 *  @amuscpi{MEASure:ADC:CURRent[:RAW]?}
	 *  @amupanelex
	 *  MEASure:ADC:CURRent?
	 *  0.152300
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_MEAS_CH_CURRENT}
	 *  @amupanelex
	 *  float val = amu.query<float>(CMD_MEAS_CH_CURRENT);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_MEAS_CH_CURRENT =					CMD_MEAS_CH_CMD_OFFSET + 0x01,
	
	/** @amutitle{Measure — ADC Temperature Sensor}
	 *  @amudesc{Measures primary temperature sensor.}
	 *  @return Temperature measurement from primary sensor
	 *
	 *  @amupanels
	 *  @amuscpi{MEASure:ADC:TSENSor[:RAW]?}
	 *  @amupanelex
	 *  MEASure:ADC:TSENSor?
	 *  25.300000
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_MEAS_CH_TSENSOR}
	 *  @amupanelex
	 *  float val = amu.query<float>(CMD_MEAS_CH_TSENSOR);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_MEAS_CH_TSENSOR =					CMD_MEAS_CH_CMD_OFFSET + 0x02,
	
	/** @amutitle{Measure — ADC TSENSOR0}
	 *  @amudesc{Measures temperature sensor 0.}
	 *  @return Temperature measurement from sensor 0
	 *
	 *  @amupanels
	 *  @amuscpi{MEASure:ADC:TSENSOR0[:RAW]?}
	 *  @amupanelex
	 *  MEASure:ADC:TSENSOR0?
	 *  25.300000
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_MEAS_CH_TSENSOR_0}
	 *  @amupanelex
	 *  float val = amu.query<float>(CMD_MEAS_CH_TSENSOR_0);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_MEAS_CH_TSENSOR_0 =					CMD_MEAS_CH_CMD_OFFSET + 0x02,
	
	/** @amutitle{Measure — ADC TSENSOR1}
	 *  @amudesc{Measures temperature sensor 1.}
	 *  @return Temperature measurement from sensor 1
	 *
	 *  @amupanels
	 *  @amuscpi{MEASure:ADC:TSENSOR1[:RAW]?}
	 *  @amupanelex
	 *  MEASure:ADC:TSENSOR1?
	 *  25.300000
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_MEAS_CH_TSENSOR_1}
	 *  @amupanelex
	 *  float val = amu.query<float>(CMD_MEAS_CH_TSENSOR_1);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_MEAS_CH_TSENSOR_1 =					CMD_MEAS_CH_CMD_OFFSET + 0x03,
	
	/** @amutitle{Measure — ADC TSENSOR2}
	 *  @amudesc{Measures temperature sensor 2.}
	 *  @return Temperature measurement from sensor 2
	 *
	 *  @amupanels
	 *  @amuscpi{MEASure:ADC:TSENSOR2[:RAW]?}
	 *  @amupanelex
	 *  MEASure:ADC:TSENSOR2?
	 *  25.300000
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_MEAS_CH_TSENSOR_2}
	 *  @amupanelex
	 *  float val = amu.query<float>(CMD_MEAS_CH_TSENSOR_2);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_MEAS_CH_TSENSOR_2 =					CMD_MEAS_CH_CMD_OFFSET + 0x04,
	
	/** @amutitle{Measure — ADC BIAS}
	 *  @amudesc{Measures bias voltage reference.}
	 *  @return Bias voltage reference measurement
	 *
	 *  @amupanels
	 *  @amuscpi{MEASure:ADC:BIAS[:RAW]?}
	 *  @amupanelex
	 *  MEASure:ADC:BIAS?
	 *  0.000100
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_MEAS_CH_BIAS}
	 *  @amupanelex
	 *  float val = amu.query<float>(CMD_MEAS_CH_BIAS);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_MEAS_CH_BIAS =						CMD_MEAS_CH_CMD_OFFSET + 0x05,
	
	/** @amutitle{Measure — ADC Offset}
	 *  @amudesc{Measures offset calibration reference.}
	 *  @return Offset calibration reference measurement
	 *
	 *  @amupanels
	 *  @amuscpi{MEASure:ADC:OFFset[:RAW]?}
	 *  @amupanelex
	 *  MEASure:ADC:OFFset?
	 *  0.000000
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_MEAS_CH_OFFSET}
	 *  @amupanelex
	 *  float val = amu.query<float>(CMD_MEAS_CH_OFFSET);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_MEAS_CH_OFFSET =					CMD_MEAS_CH_CMD_OFFSET + 0x06,
	
	/** @amutitle{Measure — ADC Temperature}
	 *  @amudesc{Measures internal MCU temperature.}
	 *  @return Internal MCU temperature measurement
	 *
	 *  @amupanels
	 *  @amuscpi{MEASure:ADC:TEMP[:RAW]?}
	 *  @amupanelex
	 *  MEASure:ADC:TEMP?
	 *  25.000000
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_MEAS_CH_TEMP}
	 *  @amupanelex
	 *  float val = amu.query<float>(CMD_MEAS_CH_TEMP);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_MEAS_CH_TEMP =						CMD_MEAS_CH_CMD_OFFSET + 0x07,
	
	/** @amutitle{Measure — ADC AVDD}
	 *  @amudesc{Measures analog supply voltage.}
	 *  @return Analog supply voltage measurement
	 *
	 *  @amupanels
	 *  @amuscpi{MEASure:ADC:AVDD[:RAW]?}
	 *  @amupanelex
	 *  MEASure:ADC:AVDD?
	 *  3.300000
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_MEAS_CH_AVDD}
	 *  @amupanelex
	 *  float val = amu.query<float>(CMD_MEAS_CH_AVDD);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_MEAS_CH_AVDD =						CMD_MEAS_CH_CMD_OFFSET + 0x08,
	
	/** @amutitle{Measure — ADC IOVDD}
	 *  @amudesc{Measures I/O supply voltage.}
	 *  @return I/O supply voltage measurement
	 *
	 *  @amupanels
	 *  @amuscpi{MEASure:ADC:IOVDD[:RAW]?}
	 *  @amupanelex
	 *  MEASure:ADC:IOVDD?
	 *  3.300000
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_MEAS_CH_IOVDD}
	 *  @amupanelex
	 *  float val = amu.query<float>(CMD_MEAS_CH_IOVDD);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_MEAS_CH_IOVDD =						CMD_MEAS_CH_CMD_OFFSET + 0x09,
	
	/** @amutitle{Measure — ADC ALDO}
	 *  @amudesc{Measures analog LDO output voltage.}
	 *  @return Analog LDO regulator voltage measurement
	 *
	 *  @amupanels
	 *  @amuscpi{MEASure:ADC:ALDO[:RAW]?}
	 *  @amupanelex
	 *  MEASure:ADC:ALDO?
	 *  1.800000
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_MEAS_CH_ALDO}
	 *  @amupanelex
	 *  float val = amu.query<float>(CMD_MEAS_CH_ALDO);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_MEAS_CH_ALDO =						CMD_MEAS_CH_CMD_OFFSET + 0x0A,
	
	/** @amutitle{Measure — ADC DLDO}
	 *  @amudesc{Measures digital LDO output voltage.}
	 *  @return Digital LDO regulator voltage measurement
	 *
	 *  @amupanels
	 *  @amuscpi{MEASure:ADC:DLDO[:RAW]?}
	 *  @amupanelex
	 *  MEASure:ADC:DLDO?
	 *  1.800000
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_MEAS_CH_DLDO}
	 *  @amupanelex
	 *  float val = amu.query<float>(CMD_MEAS_CH_DLDO);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_MEAS_CH_DLDO =						CMD_MEAS_CH_CMD_OFFSET + 0x0B,
	
	/** @amutitle{Measure — ADC SSTL}
	 *  @amudesc{Measures sun sensor top-left quadrant.}
	 *  @return Sun sensor top-left quadrant measurement
	 *
	 *  @amupanels
	 *  @amuscpi{MEASure:ADC:SSTL[:RAW]?}
	 *  @amupanelex
	 *  MEASure:ADC:SSTL?
	 *  0.250000
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_MEAS_CH_SS_TL}
	 *  @amupanelex
	 *  float val = amu.query<float>(CMD_MEAS_CH_SS_TL);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_MEAS_CH_SS_TL =						CMD_MEAS_CH_CMD_OFFSET + 0x0C,
	
	/** @amutitle{Measure — ADC SSBL}
	 *  @amudesc{Measures sun sensor bottom-left quadrant.}
	 *  @return Sun sensor bottom-left quadrant measurement
	 *
	 *  @amupanels
	 *  @amuscpi{MEASure:ADC:SSBL[:RAW]?}
	 *  @amupanelex
	 *  MEASure:ADC:SSBL?
	 *  0.250000
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_MEAS_CH_SS_BL}
	 *  @amupanelex
	 *  float val = amu.query<float>(CMD_MEAS_CH_SS_BL);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_MEAS_CH_SS_BL=						CMD_MEAS_CH_CMD_OFFSET + 0x0D,
	
	/** @amutitle{Measure — ADC SSBR}
	 *  @amudesc{Measures sun sensor bottom-right quadrant.}
	 *  @return Sun sensor bottom-right quadrant measurement
	 *
	 *  @amupanels
	 *  @amuscpi{MEASure:ADC:SSBR[:RAW]?}
	 *  @amupanelex
	 *  MEASure:ADC:SSBR?
	 *  0.250000
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_MEAS_CH_SS_BR}
	 *  @amupanelex
	 *  float val = amu.query<float>(CMD_MEAS_CH_SS_BR);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_MEAS_CH_SS_BR =						CMD_MEAS_CH_CMD_OFFSET + 0x0E,
	
	/** @amutitle{Measure — ADC SSTR}
	 *  @amudesc{Measures sun sensor top-right quadrant.}
	 *  @return Sun sensor top-right quadrant measurement
	 *
	 *  @amupanels
	 *  @amuscpi{MEASure:ADC:SSTR[:RAW]?}
	 *  @amupanelex
	 *  MEASure:ADC:SSTR?
	 *  0.250000
	 *  @endamupanelex
	 *  @endamupanel
	 *  @amui2c{CMD_MEAS_CH_SS_TR}
	 *  @amupanelex
	 *  float val = amu.query<float>(CMD_MEAS_CH_SS_TR);
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_MEAS_CH_SS_TR =						CMD_MEAS_CH_CMD_OFFSET + 0x0F,
} CMD_MEAS_CH_t;
#undef CMD_MEAS_CH_CMD_OFFSET


/**
 * @brief USB-only system command identifiers
 * @ingroup cmd_system
 *
 * These commands are only available via USB interface and provide
 * extended system functionality not accessible over I2C.
 */
typedef enum {
	/** @amutitle{System — Bootloader}
	 *  @amuusbonly
	 *  @amudesc{Enter bootloader mode.}
	 *
	 *  @amupanels
	 *  @amuscpi{SYSTem:BOOTloader}
	 *  @amupanelex
	 *  SYSTem:BOOTloader
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_USB_SYSTEM_ENTER_BOOTLOADER =		CMD_USB_SYSTEM_OFFSET + 0x00,
	
	/** @amutitle{System — SCPI List}
	 *  @amuusbonly
	 *  @amudesc{Lists every registered SCPI command (interactive discovery aid).}
	 *  @return Newline-separated list of all supported SCPI command patterns
	 *
	 *  @amupanels
	 *  @amuscpi{SYSTem:SCPI:LIST?}
	 *  @amupanelex
	 *  SYSTem:SCPI:LIST?
	 *  *IDN?
	 *  *RST
	 *  SYSTem:FIRMware?
	 *  ...
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_USB_SYSTEM_LIST_SCPI_COMMANDS =		CMD_USB_SYSTEM_OFFSET + 0x01,
	
	/** @amutitle{System — TWI Scan}
	 *  @amuusbonly
	 *  @amudesc{Scans the I2C bus and reports every address that acknowledges.}
	 *  @return Comma-separated list of responding 7-bit I2C addresses (hex)
	 *
	 *  @amupanels
	 *  @amuscpi{SYSTem:TWI:SCAN?}
	 *  @amupanelex
	 *  SYSTem:TWI:SCAN?
	 *  0x0B,0x40,0x68
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_USB_SYSTEM_TWI_SCAN =				CMD_USB_SYSTEM_OFFSET + 0x02,
	
	/** @amutitle{System — TWI Mode}
	 *  @amuusbonly
	 *  @amudesc{Sets or queries the I2C bus mode (e.g. master/slave role).}
	 *  @param mode Mode value to set (omit to query)
	 *  @return Current I2C mode
	 *
	 *  @amupanels
	 *  @amuscpi{SYSTem:TWI:MODE[?]}
	 *  @amupanelex
	 *  SYSTem:TWI:MODE 1
	 *  SYSTem:TWI:MODE?
	 *  1
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_USB_SYSTEM_TWI_MODE =				CMD_USB_SYSTEM_OFFSET + 0x03,
	
	/** @amutitle{System — Debug}
	 *  @amuusbonly
	 *  @amudesc{Reads or writes an indexed firmware debug float (the '#' selects the slot).}
	 *  @param # Debug slot index (replaces the '#' in the mnemonic, e.g. 0, 1, 2)
	 *  @param value Float to write (omit to query)
	 *  @return Float value held in the addressed debug slot
	 *
	 *  @amupanels
	 *  @amuscpi{SYSTem:DEBug#[?]}
	 *  @amupanelex
	 *  SYSTem:DEBug0 3.14
	 *  SYSTem:DEBug0?
	 *  3.140000
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 *  @note USB/SCPI only; slot meanings are firmware-defined (engineering use).
	 */
	CMD_USB_SYSTEM_DEBUG =					CMD_USB_SYSTEM_OFFSET + 0x04
} CMD_USB_SYSTEM_t;
#undef CMD_USB_SYSTEM_OFFSET


/**
 * @brief USB-only EEPROM command identifiers
 * @ingroup cmd_memory
 *
 * These commands provide direct EEPROM access for calibration data management.
 * Only available via USB interface for security and data integrity.
 */
typedef enum {
	/** @amutitle{Memory — Erase All}
	 *  @amuusbonly
	 *  @amudesc{Erase all EEPROM data.}
	 *
	 *  @amupanels
	 *  @amuscpi{MEMory:ERASE:ALL}
	 *  @amupanelex
	 *  MEMory:ERASE:ALL
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_USB_EEPROM_ERASE_ALL =				CMD_USB_EEPROM_CMD_OFFSET + 0x00,
	
	/** @amutitle{Memory — Erase Config}
	 *  @amuusbonly
	 *  @amudesc{Erase configuration data.}
	 *
	 *  @amupanels
	 *  @amuscpi{MEMory:ERASE:CONFig}
	 *  @amupanelex
	 *  MEMory:ERASE:CONFig
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_USB_EEPROM_ERASE_CONFIG =			CMD_USB_EEPROM_CMD_OFFSET + 0x01,
	
	/** @amutitle{Memory — ADC Channel Offset}
	 *  @amuusbonly
	 *  @amudesc{ADC channel offset calibration data.}
	 *  @param offset ADC channel offset calibration value to store (uint32_t; omit to query)
	 *  @return Stored ADC channel offset calibration value (uint32_t)
	 *
	 *  @amupanels
	 *  @amuscpi{MEMory:ADC:CH#:OFFset[?]}
	 *  @amupanelex
	 *  MEMory:ADC:CH0:OFFset 8388608
	 *  MEMory:ADC:CH0:OFFset?
	 *  8388608
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_USB_EEPROM_OFFSET =					CMD_USB_EEPROM_CMD_OFFSET + 0x02,
	
	/** @amutitle{Memory — ADC Channel Gain}
	 *  @amuusbonly
	 *  @amudesc{ADC channel gain calibration data.}
	 *  @param gain ADC channel gain calibration value to store (uint32_t; omit to query)
	 *  @return Stored ADC channel gain calibration value (uint32_t)
	 *
	 *  @amupanels
	 *  @amuscpi{MEMory:ADC:CH#:GAIN[?]}
	 *  @amupanelex
	 *  MEMory:ADC:CH0:GAIN 8388608
	 *  MEMory:ADC:CH0:GAIN?
	 *  8388608
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_USB_EEPROM_GAIN =					CMD_USB_EEPROM_CMD_OFFSET + 0x03,
	
	/** @amutitle{Memory — Voltage Offset}
	 *  @amuusbonly
	 *  @amudesc{Voltage measurement offset calibration.}
	 *  @param offset Voltage offset calibration value to store (uint32_t; omit to query)
	 *  @return Stored voltage offset calibration value (uint32_t)
	 *
	 *  @amupanels
	 *  @amuscpi{MEMory:VOLTage:OFFset#[?]}
	 *  @amupanelex
	 *  MEMory:VOLTage:OFFset0 8388608
	 *  MEMory:VOLTage:OFFset0?
	 *  8388608
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_USB_EEPROM_VOLTAGE_OFFSET =			CMD_USB_EEPROM_CMD_OFFSET + 0x04,
	
	/** @amutitle{Memory — Current Offset}
	 *  @amuusbonly
	 *  @amudesc{Current measurement offset calibration.}
	 *  @param offset Current offset calibration value to store (uint32_t; omit to query)
	 *  @return Stored current offset calibration value (uint32_t)
	 *
	 *  @amupanels
	 *  @amuscpi{MEMory:CURRent:OFFset#[?]}
	 *  @amupanelex
	 *  MEMory:CURRent:OFFset0 8388608
	 *  MEMory:CURRent:OFFset0?
	 *  8388608
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_USB_EEPROM_CURRENT_OFFSET =			CMD_USB_EEPROM_CMD_OFFSET + 0x05,
	
	/** @amutitle{Memory — Voltage Gain}
	 *  @amuusbonly
	 *  @amudesc{Voltage measurement gain calibration.}
	 *  @param gain Voltage gain calibration value to store (uint32_t; omit to query)
	 *  @return Stored voltage gain calibration value (uint32_t)
	 *
	 *  @amupanels
	 *  @amuscpi{MEMory:VOLTage:GAIN#[?]}
	 *  @amupanelex
	 *  MEMory:VOLTage:GAIN0 8388608
	 *  MEMory:VOLTage:GAIN0?
	 *  8388608
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_USB_EEPROM_VOLTAGE_GAIN =			CMD_USB_EEPROM_CMD_OFFSET + 0x06,
	
	/** @amutitle{Memory — Current Gain}
	 *  @amuusbonly
	 *  @amudesc{Current measurement gain calibration.}
	 *  @param gain Current gain calibration value to store (uint32_t; omit to query)
	 *  @return Stored current gain calibration value (uint32_t)
	 *
	 *  @amupanels
	 *  @amuscpi{MEMory:CURRent:GAIN#[?]}
	 *  @amupanelex
	 *  MEMory:CURRent:GAIN0 8388608
	 *  MEMory:CURRent:GAIN0?
	 *  8388608
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_USB_EEPROM_CURRENT_GAIN =			CMD_USB_EEPROM_CMD_OFFSET + 0x07,
} CMD_USB_EEPROM_t;
#undef CMD_USB_EEPROM_CMD_OFFSET

/**
 * @brief USB-only voltage ADC command identifiers
 * @ingroup cmd_adc
 *
 * These commands provide direct voltage channel control and calibration.
 * Extended functionality only available via USB interface.
 */
typedef enum {
	/** @amutitle{ADC — Voltage Calibrate Zero}
	 *  @amuusbonly
	 *  @amudesc{Voltage channel zero-scale calibration.}
	 *
	 *  @amupanels
	 *  @amuscpi{ADC:VOLTage:CALibrate:ZERO}
	 *  @amupanelex
	 *  ADC:VOLTage:CALibrate:ZERO
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_USB_ADC_VOLTAGE_CAL_ZERO =		CMD_USB_ADC_VOLTAGE_CMD_OFFSET + 0x00,
	
	/** @amutitle{ADC — Voltage Calibrate Full}
	 *  @amuusbonly
	 *  @amudesc{Voltage channel full-scale calibration.}
	 *
	 *  @amupanels
	 *  @amuscpi{ADC:VOLTage:CALibrate:FULL}
	 *  @amupanelex
	 *  ADC:VOLTage:CALibrate:FULL
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_USB_ADC_VOLTAGE_CAL_FULL =		CMD_USB_ADC_VOLTAGE_CMD_OFFSET + 0x01,
	
	/** @amutitle{ADC — Voltage Calibrate Reset}
	 *  @amuusbonly
	 *  @amudesc{Reset voltage channel calibration.}
	 *
	 *  @amupanels
	 *  @amuscpi{ADC:VOLTage:CALibrate:RESet}
	 *  @amupanelex
	 *  ADC:VOLTage:CALibrate:RESet
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_USB_ADC_VOLTAGE_CAL_RESET =		CMD_USB_ADC_VOLTAGE_CMD_OFFSET + 0x02,
	
	/** @amutitle{ADC — Voltage Calibrate Save}
	 *  @amuusbonly
	 *  @amudesc{Save voltage channel calibration.}
	 *
	 *  @amupanels
	 *  @amuscpi{ADC:VOLTage:CALibrate:SAVe}
	 *  @amupanelex
	 *  ADC:VOLTage:CALibrate:SAVe
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_USB_ADC_VOLTAGE_CAL_SAVE =		CMD_USB_ADC_VOLTAGE_CMD_OFFSET + 0x03,
	
	/** @amutitle{ADC — Voltage PGA}
	 *  @amuusbonly
	 *  @amudesc{Voltage channel PGA setting.}
	 *  @param gain PGA gain setting to write (uint8_t gain index; omit to query)
	 *  @return Current PGA gain setting (uint8_t gain index)
	 *
	 *  @amupanels
	 *  @amuscpi{ADC:VOLTage:PGA[?]}
	 *  @amupanelex
	 *  ADC:VOLTage:PGA 4
	 *  ADC:VOLTage:PGA?
	 *  4
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_USB_ADC_VOLTAGE_PGA =			CMD_USB_ADC_VOLTAGE_CMD_OFFSET + 0x04,
	
	/** @amutitle{ADC — Voltage Max}
	 *  @amuusbonly
	 *  @amudesc{Full-scale input voltage for the voltage channel (range used to scale readings).}
	 *  @param value Maximum voltage to set, in volts (omit to query)
	 *  @return Maximum input voltage, in volts
	 *
	 *  @amupanels
	 *  @amuscpi{ADC:VOLTage:MAX[?]}
	 *  @amupanelex
	 *  ADC:VOLTage:MAX?
	 *  2.500000
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_USB_ADC_VOLTAGE_MAX =			CMD_USB_ADC_VOLTAGE_CMD_OFFSET + 0x05,
	
	/** @amutitle{ADC — Voltage Offset}
	 *  @amuusbonly
	 *  @amudesc{Voltage channel offset coefficient.}
	 *  @param offset Offset calibration coefficient to write (int32_t; omit to query)
	 *  @return Voltage offset calibration coefficient (int32_t format)
	 *
	 *  @amupanels
	 *  @amuscpi{ADC:VOLTage:OFFset[?]}
	 *  @amupanelex
	 *  ADC:VOLTage:OFFset -512
	 *  ADC:VOLTage:OFFset?
	 *  -512
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_USB_ADC_VOLTAGE_OFFSET =		CMD_USB_ADC_VOLTAGE_CMD_OFFSET + 0x06,
	
	/** @amutitle{ADC — Voltage Gain}
	 *  @amuusbonly
	 *  @amudesc{Voltage channel gain coefficient.}
	 *  @param gain Gain calibration coefficient to write (uint32_t; omit to query)
	 *  @return Voltage gain calibration coefficient (uint32_t format)
	 *
	 *  @amupanels
	 *  @amuscpi{ADC:VOLTage:GAIN[?]}
	 *  @amupanelex
	 *  ADC:VOLTage:GAIN 8388608
	 *  ADC:VOLTage:GAIN?
	 *  8388608
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_USB_ADC_VOLTAGE_GAIN =			CMD_USB_ADC_VOLTAGE_CMD_OFFSET + 0x07,

	/** @amutitle{ADC — Voltage Max PGA (config)}
	 *  @amuusbonly
	 *  @amudesc{Full-scale input voltage for a specific PGA gain (the '#' selects the gain).}
	 *  @param # PGA gain index (replaces the '#' in the mnemonic, e.g. 0, 1, 2)
	 *  @param value Maximum voltage to set, in volts (omit to query)
	 *  @return Maximum input voltage for that PGA setting, in volts
	 *
	 *  @amupanels
	 *  @amuscpi{ADC:VOLTage:MAX:PGA#[?]}
	 *  @amupanelex
	 *  ADC:VOLTage:MAX:PGA0?
	 *  2.500000
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_USB_ADC_VOLTAGE_MAX_PGA = 		CMD_USB_ADC_VOLTAGE_CMD_OFFSET + 0x08,

	/** @amutitle{ADC — Voltage PGA Save}
	 *  @amuusbonly
	 *  @amudesc{Voltage channel save pga settings.}
	 *
	 *  @amupanels
	 *  @amuscpi{ADC:VOLTage:PGA:SAVE}
	 *  @amupanelex
	 *  ADC:VOLTage:PGA:SAVE
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_USB_ADC_VOLTAGE_PGA_SAVE =		CMD_USB_ADC_VOLTAGE_CMD_OFFSET + 0x09,
} CMD_USB_ADC_VOLTAGE_t;
#undef CMD_USB_ADC_VOLTAGE_CMD_OFFSET


/**
 * @brief USB-only current ADC command identifiers
 * @ingroup cmd_adc
 *
 * These commands provide direct current channel control and calibration.
 * Extended functionality only available via USB interface.
 */
typedef enum {
	/** @amutitle{ADC — Current Calibrate Zero}
	 *  @amuusbonly
	 *  @amudesc{Current channel zero-scale calibration.}
	 *
	 *  @amupanels
	 *  @amuscpi{ADC:CURRent:CALibrate:ZERO}
	 *  @amupanelex
	 *  ADC:CURRent:CALibrate:ZERO
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_USB_ADC_CURRENT_CAL_ZERO =		CMD_USB_ADC_CURRENT_CMD_OFFSET + 0x00,
	
	/** @amutitle{ADC — Current Calibrate Full}
	 *  @amuusbonly
	 *  @amudesc{Current channel full-scale calibration.}
	 *
	 *  @amupanels
	 *  @amuscpi{ADC:CURRent:CALibrate:FULL}
	 *  @amupanelex
	 *  ADC:CURRent:CALibrate:FULL
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_USB_ADC_CURRENT_CAL_FULL =		CMD_USB_ADC_CURRENT_CMD_OFFSET + 0x01,
	
	/** @amutitle{ADC — Current Calibrate Reset}
	 *  @amuusbonly
	 *  @amudesc{Reset current channel calibration.}
	 *
	 *  @amupanels
	 *  @amuscpi{ADC:CURRent:CALibrate:RESet}
	 *  @amupanelex
	 *  ADC:CURRent:CALibrate:RESet
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_USB_ADC_CURRENT_CAL_RESET =		CMD_USB_ADC_CURRENT_CMD_OFFSET + 0x02,
	
	/** @amutitle{ADC — Current Calibrate Save}
	 *  @amuusbonly
	 *  @amudesc{Save current channel calibration.}
	 *
	 *  @amupanels
	 *  @amuscpi{ADC:CURRent:CALibrate:SAVe}
	 *  @amupanelex
	 *  ADC:CURRent:CALibrate:SAVe
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_USB_ADC_CURRENT_CAL_SAVE =		CMD_USB_ADC_CURRENT_CMD_OFFSET + 0x03,
	
	/** @amutitle{ADC — Current PGA}
	 *  @amuusbonly
	 *  @amudesc{Current channel PGA setting.}
	 *  @param gain PGA gain setting to write (uint8_t gain index; omit to query)
	 *  @return Current PGA gain setting (uint8_t gain index)
	 *
	 *  @amupanels
	 *  @amuscpi{ADC:CURRent:PGA[?]}
	 *  @amupanelex
	 *  ADC:CURRent:PGA 4
	 *  ADC:CURRent:PGA?
	 *  4
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_USB_ADC_CURRENT_PGA =			CMD_USB_ADC_CURRENT_CMD_OFFSET + 0x04,
	
	/** @amutitle{ADC — Current Max}
	 *  @amuusbonly
	 *  @amudesc{Full-scale input current for the current channel (range used to scale readings).}
	 *  @param value Maximum current to set, in amperes (omit to query)
	 *  @return Maximum input current, in amperes
	 *
	 *  @amupanels
	 *  @amuscpi{ADC:CURRent:MAX[?]}
	 *  @amupanelex
	 *  ADC:CURRent:MAX?
	 *  0.500000
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_USB_ADC_CURRENT_MAX =			CMD_USB_ADC_CURRENT_CMD_OFFSET + 0x05,
	
	/** @amutitle{ADC — Current Offset}
	 *  @amuusbonly
	 *  @amudesc{Current channel offset coefficient.}
	 *  @param offset Offset calibration coefficient to write (int32_t; omit to query)
	 *  @return Current offset calibration coefficient (int32_t format)
	 *
	 *  @amupanels
	 *  @amuscpi{ADC:CURRent:OFFset[?]}
	 *  @amupanelex
	 *  ADC:CURRent:OFFset -512
	 *  ADC:CURRent:OFFset?
	 *  -512
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_USB_ADC_CURRENT_OFFSET =		CMD_USB_ADC_CURRENT_CMD_OFFSET + 0x06,
	
	/** @amutitle{ADC — Current Gain}
	 *  @amuusbonly
	 *  @amudesc{Current channel gain coefficient.}
	 *  @param gain Gain calibration coefficient to write (uint32_t; omit to query)
	 *  @return Current gain calibration coefficient (uint32_t format)
	 *
	 *  @amupanels
	 *  @amuscpi{ADC:CURRent:GAIN[?]}
	 *  @amupanelex
	 *  ADC:CURRent:GAIN 8388608
	 *  ADC:CURRent:GAIN?
	 *  8388608
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_USB_ADC_CURRENT_GAIN =			CMD_USB_ADC_CURRENT_CMD_OFFSET + 0x07,

	/** @amutitle{ADC — Current Max PGA (config)}
	 *  @amuusbonly
	 *  @amudesc{Full-scale input current for a specific PGA gain (the '#' selects the gain).}
	 *  @param # PGA gain index (replaces the '#' in the mnemonic, e.g. 0, 1, 2)
	 *  @param value Maximum current to set, in amperes (omit to query)
	 *  @return Maximum input current for that PGA setting, in amperes
	 *
	 *  @amupanels
	 *  @amuscpi{ADC:CURRent:MAX:PGA#[?]}
	 *  @amupanelex
	 *  ADC:CURRent:MAX:PGA0?
	 *  0.500000
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_USB_ADC_CURRENT_MAX_PGA = 		CMD_USB_ADC_CURRENT_CMD_OFFSET + 0x08,

	/** @amutitle{ADC — Current PGA Save}
	 *  @amuusbonly
	 *  @amudesc{Current channel save pga settings.}
	 *
	 *  @amupanels
	 *  @amuscpi{ADC:CURRent:PGA:SAVE}
	 *  @amupanelex
	 *  ADC:CURRent:PGA:SAVE
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_USB_ADC_CURRENT_PGA_SAVE =		CMD_USB_ADC_CURRENT_CMD_OFFSET + 0x09,
} CMD_USB_ADC_CURRENT_t;
#undef CMD_USB_ADC_CURRENT_CMD_OFFSET

/**
 * @brief USB-only sweep command identifiers
 * @ingroup cmd_sweep
 *
 * These commands provide USB-specific sweep functionality.
 */
typedef enum {
	/** @amutitle{Sweep — Trigger USB}
	 *  @amuusbonly
	 *  @amudesc{Trigger sweep via USB.}
	 *
	 *  @amupanels
	 *  @amuscpi{SWEEP:TRIGger:USB}
	 *  @amupanelex
	 *  SWEEP:TRIGger:USB
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_USB_SWEEP_TRIGGER =					CMD_USB_SWEEP_OFFSET + 0x00,
} CMD_USB_SWEEP_t;
#undef CMD_USB_SWEEP_OFFSET

/**
 * @brief USB-only system configuration command identifiers
 * @ingroup cmd_system
 *
 * These commands configure system-level hardware parameters and save configuration.
 * Only available via USB for security and to prevent accidental misconfiguration.
 */
typedef enum {
	/** @amutitle{System — Config Current Gain}
	 *  @amuusbonly
	 *  @amudesc{Current measurement gain setting.}
	 *  @param gain Current-measurement gain factor to write (float; omit to query)
	 *  @return Current-measurement gain factor (float)
	 *
	 *  @amupanels
	 *  @amuscpi{SYSTem:CONFig:CURRent:GAIN[?]}
	 *  @amupanelex
	 *  SYSTem:CONFig:CURRent:GAIN 1.0
	 *  SYSTem:CONFig:CURRent:GAIN?
	 *  1.000000
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_USB_SYSTEM_CONFIG_CURR_GAIN =		CMD_USB_SYSTEM_CONFIG_OFFSET + 0x00,
	
	/** @amutitle{System — Config Current Sense Resistor}
	 *  @amuusbonly
	 *  @amudesc{Current measurement sense resistor value.}
	 *  @param rsense Current sense resistor value to write, in ohms (float; omit to query)
	 *  @return Current sense resistor value, in ohms (float)
	 *
	 *  @amupanels
	 *  @amuscpi{SYSTem:CONFig:CURRent:Rsense[?]}
	 *  @amupanelex
	 *  SYSTem:CONFig:CURRent:Rsense 0.05
	 *  SYSTem:CONFig:CURRent:Rsense?
	 *  0.050000
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_USB_SYSTEM_CONFIG_CURR_RSENSE =		CMD_USB_SYSTEM_CONFIG_OFFSET + 0x01,
	
	/** @amutitle{System — Config Voltage R1}
	 *  @amuusbonly
	 *  @amudesc{Voltage divider R1 resistor value.}
	 *  @param r1 Voltage-divider R1 resistance to write, in ohms (float; omit to query)
	 *  @return Voltage-divider R1 resistance, in ohms (float)
	 *
	 *  @amupanels
	 *  @amuscpi{SYSTem:CONFig:VOLTage:R1[?]}
	 *  @amupanelex
	 *  SYSTem:CONFig:VOLTage:R1 100000.0
	 *  SYSTem:CONFig:VOLTage:R1?
	 *  100000.000000
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_USB_SYSTEM_CONFIG_VOLT_R1 =			CMD_USB_SYSTEM_CONFIG_OFFSET + 0x02,
	
	/** @amutitle{System — Config Voltage R2}
	 *  @amuusbonly
	 *  @amudesc{Voltage divider R2 resistor value.}
	 *  @param r2 Voltage-divider R2 resistance to write, in ohms (float; omit to query)
	 *  @return Voltage-divider R2 resistance, in ohms (float)
	 *
	 *  @amupanels
	 *  @amuscpi{SYSTem:CONFig:VOLTage:R2[?]}
	 *  @amupanelex
	 *  SYSTem:CONFig:VOLTage:R2 10000.0
	 *  SYSTem:CONFig:VOLTage:R2?
	 *  10000.000000
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_USB_SYSTEM_CONFIG_VOLT_R2 =			CMD_USB_SYSTEM_CONFIG_OFFSET + 0x03,
	
	/** @amutitle{System — Config Save}
	 *  @amuusbonly
	 *  @amudesc{Save system configuration.}
	 *
	 *  @amupanels
	 *  @amuscpi{SYSTem:CONFig:SAVE}
	 *  @amupanelex
	 *  SYSTem:CONFig:SAVE
	 *  @endamupanelex
	 *  @endamupanel
	 *  @endamupanels
	 */
	CMD_USB_SYSTEM_CONFIG_SAVE = 			CMD_USB_SYSTEM_CONFIG_OFFSET + 0x07,
} CMD_USB_SYSTEM_CONFIG_t;
#undef CMD_USB_SYSTEM_CONFIG_OFFSET

#endif /* __AMU_COMMANDS_H__ */