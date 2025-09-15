/**
 * @file scpi.h
 * @brief TODO
 * 
 * TODO: explain what the commands are for
 *
 * @author	CJM28241
 * @date	5/28/2018 7:13:29 PM
 */ 


#ifndef __SCPI_H__
#define __SCPI_H__

#include "libscpi\libscpi.h"
#include "amu_commands.h"


#define SCPI_IDN_STRING_LENGTH			32

#define SCPI_IDN_DEVICETYPE_STR			0
#define SCPI_IDN_MANUFACTURER_STR		1
#define SCPI_IDN_SERIALNUM_STR			2
#define SCPI_IDN_FIRMWARE_STR			3

#ifdef	__cplusplus
extern "C" {
#endif

	void amu_scpi_init(volatile amu_device_t *dev, const char * idn1, const char * idn2, const char * idn3, const char * idn4);
	void amu_scpi_update(const char incomingByte);
    void amu_scpi_update_buffer(const char* buffer, size_t len);
	void amu_scpi_list_commands(void);
	void amu_scpi_add_aux_commands(const scpi_command_t* aux_cmd_list);
	scpi_result_t scpi_cmd_execute(scpi_t *context);
	
	
	#define SCPI_CMD_RW_PROTOTYPE(TYPE)	scpi_result_t scpi_cmd_rw_##TYPE(scpi_t *context);
	SCPI_CMD_RW_PROTOTYPE(uint8_t)
	SCPI_CMD_RW_PROTOTYPE(uint16_t)
	SCPI_CMD_RW_PROTOTYPE(uint32_t)
    SCPI_CMD_RW_PROTOTYPE(int32_t)
	SCPI_CMD_RW_PROTOTYPE(float)
	SCPI_CMD_RW_PROTOTYPE(amu_pid_t)
	SCPI_CMD_RW_PROTOTYPE(amu_coeff_t)
	SCPI_CMD_RW_PROTOTYPE(amu_notes_t)
	SCPI_CMD_RW_PROTOTYPE(ss_angle_t)
	SCPI_CMD_RW_PROTOTYPE(press_data_t)
	SCPI_CMD_RW_PROTOTYPE(amu_int_volt_t)
	SCPI_CMD_RW_PROTOTYPE(amu_meas_t)

	#define SCPI_CMD_EXEC_QRY_PROTOTYPE(TYPE)	scpi_result_t scpi_cmd_exec_qry_##TYPE(scpi_t *context);
	SCPI_CMD_EXEC_QRY_PROTOTYPE(uint8_t)
	SCPI_CMD_EXEC_QRY_PROTOTYPE(uint16_t)
	SCPI_CMD_EXEC_QRY_PROTOTYPE(uint32_t)
	SCPI_CMD_EXEC_QRY_PROTOTYPE(float)
	
	scpi_result_t _scpi_read_ptr(scpi_t *context);

	scpi_result_t _scpi_write_sweep_ptr(scpi_t *context);
	scpi_result_t _scpi_write_config_ptr(scpi_t *context);
	scpi_result_t _scpi_write_meta_ptr(scpi_t *context);

	scpi_result_t _scpi_cmd_twi_scan(scpi_t *context);
	scpi_result_t _scpi_cmd_measure_channel(scpi_t *context);
	scpi_result_t _scpi_cmd_measure_active_ch(scpi_t *context);
	scpi_result_t _scpi_cmd_measure_tsensors(scpi_t *context);
	scpi_result_t _scpi_cmd_query_str(scpi_t *context);
	scpi_result_t _scpi_cmd_led(scpi_t *context);

	int16_t _scpi_get_channelList(scpi_t *context);

#if defined(__AMU_USE_SCPI__)

    #ifdef SCPI_USE_PROGMEM
        #define SCPI_COMMAND(P, C, T) static const char C ## _ ## T ## _pattern[] PROGMEM = P;
    #else
        #define SCPI_COMMAND(P, C, T) static const char C ## _ ## T ## _pattern[] = P;
    #endif

        #define __AMU_DEFAULT_CMD_LIST__																							\
        /**
 * @brief Clear status command - clears device status registers
 * @ingroup scpi_ieee488_commands
 * 
 * **SCPI Command:** `*CLS`
 * 
 * **Handler:** SCPI_CoreCls  
 * **Command ID:** 0
 * 
 * @param Command parameters vary by function
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("*CLS");
 * @endcode
 */
        SCPI_COMMAND("*CLS", SCPI_CoreCls, 0)	\
        /**
 * @brief Query event status enable register
 * @ingroup scpi_ieee488_commands
 * 
 * **SCPI Command:** `*ESE?`
 * 
 * **Handler:** SCPI_CoreEseQ  
 * **Command ID:** 0
 * 
 * @param Query only - no parameters
 * @return Numeric value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("*ESE?");
 * @endcode
 */
        SCPI_COMMAND("*ESE?", SCPI_CoreEseQ, 0)	\
        /**
 * @brief Query event status register
 * @ingroup scpi_ieee488_commands
 * 
 * **SCPI Command:** `*ESR?`
 * 
 * **Handler:** SCPI_CoreEsrQ  
 * **Command ID:** 0
 * 
 * @param Query only - no parameters
 * @return Numeric value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("*ESR?");
 * @endcode
 */
        SCPI_COMMAND("*ESR?", SCPI_CoreEsrQ, 0)	\
        /**
 * @brief Query device identification string
 * @ingroup scpi_ieee488_commands
 * 
 * **SCPI Command:** `*IDN?`
 * 
 * **Handler:** SCPI_CoreIdnQ  
 * **Command ID:** 0
 * 
 * @param Query only - no parameters
 * @return Numeric value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("*IDN?");
 * @endcode
 */
        SCPI_COMMAND("*IDN?", SCPI_CoreIdnQ, 0)	\
        /**
 * @brief Operation complete command
 * @ingroup scpi_ieee488_commands
 * 
 * **SCPI Command:** `*OPC`
 * 
 * **Handler:** SCPI_CoreOpc  
 * **Command ID:** 0
 * 
 * @param Command parameters vary by function
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("*OPC");
 * @endcode
 */
        SCPI_COMMAND("*OPC", SCPI_CoreOpc, 0)	\
        /**
 * @brief Query operation complete status
 * @ingroup scpi_ieee488_commands
 * 
 * **SCPI Command:** `*OPC?`
 * 
 * **Handler:** SCPI_CoreOpcQ  
 * **Command ID:** 0
 * 
 * @param Query only - no parameters
 * @return Numeric value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("*OPC?");
 * @endcode
 */
        SCPI_COMMAND("*OPC?", SCPI_CoreOpcQ, 0)	\
        /**
 * @brief Reset device to default state
 * @ingroup scpi_ieee488_commands
 * 
 * **SCPI Command:** `*RST`
 * 
 * **Handler:** SCPI_CoreRst  
 * **Command ID:** 0
 * 
 * @param Command parameters vary by function
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("*RST");
 * @endcode
 */
        SCPI_COMMAND("*RST", SCPI_CoreRst, 0)	\
        /**
 * @brief Set service request enable register
 * @ingroup scpi_ieee488_commands
 * 
 * **SCPI Command:** `*SRE`
 * 
 * **Handler:** SCPI_CoreSre  
 * **Command ID:** 0
 * 
 * @param Command parameters vary by function
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("*SRE");
 * @endcode
 */
        SCPI_COMMAND("*SRE", SCPI_CoreSre, 0)	\
        /**
 * @brief Query service request enable register
 * @ingroup scpi_ieee488_commands
 * 
 * **SCPI Command:** `*SRE?`
 * 
 * **Handler:** SCPI_CoreSreQ  
 * **Command ID:** 0
 * 
 * @param Query only - no parameters
 * @return Numeric value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("*SRE?");
 * @endcode
 */
        SCPI_COMMAND("*SRE?", SCPI_CoreSreQ, 0)	\
        /**
 * @brief Query status byte register
 * @ingroup scpi_ieee488_commands
 * 
 * **SCPI Command:** `*STB?`
 * 
 * **Handler:** SCPI_CoreStbQ  
 * **Command ID:** 0
 * 
 * @param Query only - no parameters
 * @return Numeric value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("*STB?");
 * @endcode
 */
        SCPI_COMMAND("*STB?", SCPI_CoreStbQ, 0)	\
        /**
 * @brief Self-test query
 * @ingroup scpi_ieee488_commands
 * 
 * **SCPI Command:** `*TST?`
 * 
 * **Handler:** scpi_cmd_execute  
 * **Command ID:** CMD_SYSTEM_NO_CMD
 * 
 * @param Query only - no parameters
 * @return Numeric value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("*TST?");
 * @endcode
 */
        SCPI_COMMAND("*TST?", scpi_cmd_execute, CMD_SYSTEM_NO_CMD)	\
        /**
 * @brief Wait for all operations to complete
 * @ingroup scpi_ieee488_commands
 * 
 * **SCPI Command:** `*WAI`
 * 
 * **Handler:** SCPI_CoreWai  
 * **Command ID:** 0
 * 
 * @param Command parameters vary by function
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("*WAI");
 * @endcode
 */
        SCPI_COMMAND("*WAI", SCPI_CoreWai, 0)	\
                                                                                                                                    \
        /**
 * @brief Set status:questionable:enable value
 * @ingroup scpi_status_commands
 * 
 * **SCPI Command:** `STATus:QUEStionable:ENABle`
 * 
 * **Handler:** SCPI_StatusQuestionableEnable  
 * **Command ID:** 0
 * 
 * @param Command parameters vary by function
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("STATus:QUEStionable:ENABle");
 * @endcode
 */
        SCPI_COMMAND("STATus:QUEStionable:ENABle", SCPI_StatusQuestionableEnable, 0)	\
        /**
 * @brief Query status:questionable:enable value
 * @ingroup scpi_status_commands
 * 
 * **SCPI Command:** `STATus:QUEStionable:ENABle?`
 * 
 * **Handler:** SCPI_StatusQuestionableEnableQ  
 * **Command ID:** 0
 * 
 * @param Query only - no parameters
 * @return Numeric value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("STATus:QUEStionable:ENABle?");
 * @endcode
 */
        SCPI_COMMAND("STATus:QUEStionable:ENABle?", SCPI_StatusQuestionableEnableQ, 0)	\
        /**
 * @brief Set status:preset value
 * @ingroup scpi_status_commands
 * 
 * **SCPI Command:** `STATus:PRESet`
 * 
 * **Handler:** SCPI_StatusPreset  
 * **Command ID:** 0
 * 
 * @param Command parameters vary by function
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("STATus:PRESet");
 * @endcode
 */
        SCPI_COMMAND("STATus:PRESet", SCPI_StatusPreset, 0)	\
                                                                                                                                    \
        /**
 * @brief Query next error in error queue
 * @ingroup scpi_system_commands
 * 
 * **SCPI Command:** `SYSTem:ERRor[:NEXT]?`
 * 
 * **Handler:** SCPI_SystemErrorNextQ  
 * **Command ID:** 0
 * 
 * @param Query only - no parameters
 * @return Numeric value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SYSTem:ERRor[:NEXT]?");
 * @endcode
 */
        SCPI_COMMAND("SYSTem:ERRor[:NEXT]?", SCPI_SystemErrorNextQ, 0)	\
        /**
 * @brief Query number of errors in error queue
 * @ingroup scpi_system_commands
 * 
 * **SCPI Command:** `SYSTem:ERRor:COUNt?`
 * 
 * **Handler:** SCPI_SystemErrorCountQ  
 * **Command ID:** 0
 * 
 * @param Query only - no parameters
 * @return Numeric value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SYSTem:ERRor:COUNt?");
 * @endcode
 */
        SCPI_COMMAND("SYSTem:ERRor:COUNt?", SCPI_SystemErrorCountQ, 0)	\
        /**
 * @brief Query SCPI version
 * @ingroup scpi_system_commands
 * 
 * **SCPI Command:** `SYSTem:VERSion?`
 * 
 * **Handler:** SCPI_SystemVersionQ  
 * **Command ID:** 0
 * 
 * @param Query only - no parameters
 * @return Numeric value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SYSTem:VERSion?");
 * @endcode
 */
        SCPI_COMMAND("SYSTem:VERSion?", SCPI_SystemVersionQ, 0)	\
        /**
 * @brief Query system:scpi:list value
 * @ingroup scpi_system_commands
 * 
 * **SCPI Command:** `SYSTem:SCPI:LIST?`
 * 
 * **Handler:** scpi_cmd_execute  
 * **Command ID:** CMD_USB_SYSTEM_LIST_SCPI_COMMANDS
 * 
 * @param Query only - no parameters
 * @return Numeric value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SYSTem:SCPI:LIST?");
 * @endcode
 */
        SCPI_COMMAND("SYSTem:SCPI:LIST?", scpi_cmd_execute, CMD_USB_SYSTEM_LIST_SCPI_COMMANDS)	\
                                                                                                                                    \
        /**
 * @brief Enter bootloader mode for firmware update
 * @ingroup scpi_system_commands
 * 
 * **SCPI Command:** `SYSTem:BOOTloader`
 * 
 * **Handler:** scpi_cmd_execute  
 * **Command ID:** CMD_USB_SYSTEM_ENTER_BOOTLOADER
 * 
 * @param Command parameters vary by function
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SYSTem:BOOTloader");
 * @endcode
 */
        SCPI_COMMAND("SYSTem:BOOTloader", scpi_cmd_execute, CMD_USB_SYSTEM_ENTER_BOOTLOADER)	\
        /**
 * @brief Query system:debug# value
 * @ingroup scpi_system_commands
 * 
 * **SCPI Command:** `SYSTem:DEBug#?`
 * 
 * **Handler:** scpi_cmd_rw_float  
 * **Command ID:** CMD_USB_SYSTEM_DEBUG
 * 
 * @param Query only - no parameters
 * @return Floating-point value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SYSTem:DEBug#?");
 * @endcode
 */
        SCPI_COMMAND("SYSTem:DEBug#?", scpi_cmd_rw_float, CMD_USB_SYSTEM_DEBUG)	\
        /**
 * @brief Query firmware version string
 * @ingroup scpi_system_commands
 * 
 * **SCPI Command:** `SYSTem:FIRMware?`
 * 
 * **Handler:** _scpi_cmd_query_str  
 * **Command ID:** CMD_SYSTEM_FIRMWARE
 * 
 * @param Query only - no parameters
 * @return String value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SYSTem:FIRMware?");
 * @endcode
 */
        SCPI_COMMAND("SYSTem:FIRMware?", _scpi_cmd_query_str, CMD_SYSTEM_FIRMWARE)	\
        /**
 * @brief Query hardware revision number
 * @ingroup scpi_system_commands
 * 
 * **SCPI Command:** `SYSTem:HARDware?`
 * 
 * **Handler:** scpi_cmd_rw_uint8_t  
 * **Command ID:** AMU_REG_SYSTEM_HARDWARE_REVISION
 * 
 * @param Query only - no parameters
 * @return 8-bit unsigned integer
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SYSTem:HARDware?");
 * @endcode
 */
        SCPI_COMMAND("SYSTem:HARDware?", scpi_cmd_rw_uint8_t, AMU_REG_SYSTEM_HARDWARE_REVISION)	\
        /**
 * @brief Set LED pattern mode
 * @ingroup scpi_system_commands
 * 
 * **SCPI Command:** `SYSTem:LED:PAT`
 * 
 * **Handler:** _scpi_cmd_led  
 * **Command ID:** CMD_SYSTEM_LED
 * 
 * @param Command parameters vary by function
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SYSTem:LED:PAT");
 * @endcode
 */
        SCPI_COMMAND("SYSTem:LED:PAT", _scpi_cmd_led, CMD_SYSTEM_LED)	\
        /**
 * @brief Set or query LED RGB color values
 * @ingroup scpi_system_commands
 * 
 * **SCPI Command:** `SYSTem:LED:COLOR[?]`
 * 
 * **Handler:** scpi_cmd_rw_amu_pid_t  
 * **Command ID:** CMD_SYSTEM_LED_COLOR
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SYSTem:LED:COLOR[?]");
 * @endcode
 */
        SCPI_COMMAND("SYSTem:LED:COLOR[?]", scpi_cmd_rw_amu_pid_t, CMD_SYSTEM_LED_COLOR)	\
        /**
 * @brief Query device serial number
 * @ingroup scpi_system_commands
 * 
 * **SCPI Command:** `SYSTem:SERial?`
 * 
 * **Handler:** _scpi_cmd_query_str  
 * **Command ID:** CMD_SYSTEM_SERIAL_NUM
 * 
 * @param Query only - no parameters
 * @return String value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SYSTem:SERial?");
 * @endcode
 */
        SCPI_COMMAND("SYSTem:SERial?", _scpi_cmd_query_str, CMD_SYSTEM_SERIAL_NUM)	\
        /**
 * @brief Enter low-power sleep mode
 * @ingroup scpi_system_commands
 * 
 * **SCPI Command:** `SYSTem:SLEEP`
 * 
 * **Handler:** scpi_cmd_execute  
 * **Command ID:** CMD_SYSTEM_SLEEP
 * 
 * @param Command parameters vary by function
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SYSTem:SLEEP");
 * @endcode
 */
        SCPI_COMMAND("SYSTem:SLEEP", scpi_cmd_execute, CMD_SYSTEM_SLEEP)	\
        /**
 * @brief Query system status flags
 * @ingroup scpi_system_commands
 * 
 * **SCPI Command:** `SYSTem:STATus?`
 * 
 * **Handler:** scpi_cmd_rw_uint8_t  
 * **Command ID:** AMU_REG_SYSTEM_AMU_STATUS
 * 
 * @param Query only - no parameters
 * @return 8-bit unsigned integer
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SYSTem:STATus?");
 * @endcode
 */
        SCPI_COMMAND("SYSTem:STATus?", scpi_cmd_rw_uint8_t, AMU_REG_SYSTEM_AMU_STATUS)	\
        /**
 * @brief Query internal temperature in Celsius
 * @ingroup scpi_system_commands
 * 
 * **SCPI Command:** `SYSTem:TEMPerature?`
 * 
 * **Handler:** scpi_cmd_rw_float  
 * **Command ID:** CMD_SYSTEM_TEMPERATURE
 * 
 * @param Query only - no parameters
 * @return Floating-point value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SYSTem:TEMPerature?");
 * @endcode
 */
        SCPI_COMMAND("SYSTem:TEMPerature?", scpi_cmd_rw_float, CMD_SYSTEM_TEMPERATURE)	\
        /**
 * @brief Set or query system timestamp
 * @ingroup scpi_system_commands
 * 
 * **SCPI Command:** `SYSTem:TIMEstamp[?]`
 * 
 * **Handler:** scpi_cmd_rw_uint32_t  
 * **Command ID:** CMD_SYSTEM_TIME
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SYSTem:TIMEstamp[?]");
 * @endcode
 */
        SCPI_COMMAND("SYSTem:TIMEstamp[?]", scpi_cmd_rw_uint32_t, CMD_SYSTEM_TIME)	\
        /**
 * @brief Set or query UTC timestamp
 * @ingroup scpi_system_commands
 * 
 * **SCPI Command:** `SYSTem:TIMEstamp:UTC[?]`
 * 
 * **Handler:** scpi_cmd_rw_uint32_t  
 * **Command ID:** CMD_SYSTEM_UTC_TIME
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SYSTem:TIMEstamp:UTC[?]");
 * @endcode
 */
        SCPI_COMMAND("SYSTem:TIMEstamp:UTC[?]", scpi_cmd_rw_uint32_t, CMD_SYSTEM_UTC_TIME)	\
        /**
 * @brief Set or query I2C device address
 * @ingroup scpi_system_commands
 * 
 * **SCPI Command:** `SYSTem:TWI:ADDress[?]`
 * 
 * **Handler:** scpi_cmd_rw_uint8_t  
 * **Command ID:** CMD_SYSTEM_TWI_ADDRESS
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SYSTem:TWI:ADDress[?]");
 * @endcode
 */
        SCPI_COMMAND("SYSTem:TWI:ADDress[?]", scpi_cmd_rw_uint8_t, CMD_SYSTEM_TWI_ADDRESS)	\
        /**
 * @brief Query number of I2C devices detected
 * @ingroup scpi_system_commands
 * 
 * **SCPI Command:** `SYSTem:TWI:NUMdevices?`
 * 
 * **Handler:** scpi_cmd_rw_uint8_t  
 * **Command ID:** CMD_SYSTEM_TWI_NUM_DEVICES
 * 
 * @param Query only - no parameters
 * @return 8-bit unsigned integer
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SYSTem:TWI:NUMdevices?");
 * @endcode
 */
        SCPI_COMMAND("SYSTem:TWI:NUMdevices?", scpi_cmd_rw_uint8_t, CMD_SYSTEM_TWI_NUM_DEVICES)	\
        /**
 * @brief Set or query I2C operating mode
 * @ingroup scpi_system_commands
 * 
 * **SCPI Command:** `SYSTem:TWI:MODE[?]`
 * 
 * **Handler:** scpi_cmd_rw_uint8_t  
 * **Command ID:** CMD_USB_SYSTEM_TWI_MODE
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SYSTem:TWI:MODE[?]");
 * @endcode
 */
        SCPI_COMMAND("SYSTem:TWI:MODE[?]", scpi_cmd_rw_uint8_t, CMD_USB_SYSTEM_TWI_MODE)	\
        /**
 * @brief Scan I2C bus and return device addresses
 * @ingroup scpi_system_commands
 * 
 * **SCPI Command:** `SYSTem:TWI:SCAN?`
 * 
 * **Handler:** _scpi_cmd_twi_scan  
 * **Command ID:** CMD_USB_SYSTEM_TWI_SCAN
 * 
 * @param Query only - no parameters
 * @return Numeric value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SYSTem:TWI:SCAN?");
 * @endcode
 */
        SCPI_COMMAND("SYSTem:TWI:SCAN?", _scpi_cmd_twi_scan, CMD_USB_SYSTEM_TWI_SCAN)	\
        /**
 * @brief Query I2C interface status
 * @ingroup scpi_system_commands
 * 
 * **SCPI Command:** `SYSTem:TWI:STATus?`
 * 
 * **Handler:** scpi_cmd_rw_uint8_t  
 * **Command ID:** CMD_SYSTEM_TWI_STATUS
 * 
 * @param Query only - no parameters
 * @return 8-bit unsigned integer
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SYSTem:TWI:STATus?");
 * @endcode
 */
        SCPI_COMMAND("SYSTem:TWI:STATus?", scpi_cmd_rw_uint8_t, CMD_SYSTEM_TWI_STATUS)	\
        /**
 * @brief Query system:xmega:fuses value
 * @ingroup scpi_system_commands
 * 
 * **SCPI Command:** `SYSTem:XMEGA:FUSES?`
 * 
 * **Handler:** scpi_cmd_rw_uint32_t  
 * **Command ID:** CMD_SYSTEM_XMEGA_FUSES
 * 
 * @param Query only - no parameters
 * @return 32-bit unsigned integer
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SYSTem:XMEGA:FUSES?");
 * @endcode
 */
        SCPI_COMMAND("SYSTem:XMEGA:FUSES?", scpi_cmd_rw_uint32_t, CMD_SYSTEM_XMEGA_FUSES)	\
        /**
 * @brief Query system:xmega:signature value
 * @ingroup scpi_system_commands
 * 
 * **SCPI Command:** `SYSTem:XMEGA:SIGnature?`
 * 
 * **Handler:** _scpi_cmd_query_str  
 * **Command ID:** CMD_SYSTEM_XMEGA_USER_SIGNATURES
 * 
 * @param Query only - no parameters
 * @return String value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SYSTem:XMEGA:SIGnature?");
 * @endcode
 */
        SCPI_COMMAND("SYSTem:XMEGA:SIGnature?", _scpi_cmd_query_str, CMD_SYSTEM_XMEGA_USER_SIGNATURES)	\
                                                                                                                                    \
        /**
 * @brief Set dut:junction[?] value
 * @ingroup scpi_dut_commands
 * 
 * **SCPI Command:** `DUT:JUNCtion[?]`
 * 
 * **Handler:** scpi_cmd_rw_uint8_t  
 * **Command ID:** CMD_DUT_JUNCTION
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("DUT:JUNCtion[?]");
 * @endcode
 */
        SCPI_COMMAND("DUT:JUNCtion[?]", scpi_cmd_rw_uint8_t, CMD_DUT_JUNCTION)	\
        /**
 * @brief Set dut:coverglass[?] value
 * @ingroup scpi_dut_commands
 * 
 * **SCPI Command:** `DUT:COVERglass[?]`
 * 
 * **Handler:** scpi_cmd_rw_uint8_t  
 * **Command ID:** CMD_DUT_COVERGLASS
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("DUT:COVERglass[?]");
 * @endcode
 */
        SCPI_COMMAND("DUT:COVERglass[?]", scpi_cmd_rw_uint8_t, CMD_DUT_COVERGLASS)	\
        /**
 * @brief Set dut:interconnect[?] value
 * @ingroup scpi_dut_commands
 * 
 * **SCPI Command:** `DUT:INTERconnect[?]`
 * 
 * **Handler:** scpi_cmd_rw_uint8_t  
 * **Command ID:** CMD_DUT_INTERCONNECT
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("DUT:INTERconnect[?]");
 * @endcode
 */
        SCPI_COMMAND("DUT:INTERconnect[?]", scpi_cmd_rw_uint8_t, CMD_DUT_INTERCONNECT)	\
        /**
 * @brief Set dut:manufacturer[?] value
 * @ingroup scpi_dut_commands
 * 
 * **SCPI Command:** `DUT:MANufacturer[?]`
 * 
 * **Handler:** _scpi_cmd_query_str  
 * **Command ID:** CMD_DUT_MANUFACTURER
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("DUT:MANufacturer[?]");
 * @endcode
 */
        SCPI_COMMAND("DUT:MANufacturer[?]", _scpi_cmd_query_str, CMD_DUT_MANUFACTURER)	\
        /**
 * @brief Set dut:model[?] value
 * @ingroup scpi_dut_commands
 * 
 * **SCPI Command:** `DUT:MODel[?]`
 * 
 * **Handler:** _scpi_cmd_query_str  
 * **Command ID:** CMD_DUT_MODEL
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("DUT:MODel[?]");
 * @endcode
 */
        SCPI_COMMAND("DUT:MODel[?]", _scpi_cmd_query_str, CMD_DUT_MODEL)	\
        /**
 * @brief Set dut:technology[?] value
 * @ingroup scpi_dut_commands
 * 
 * **SCPI Command:** `DUT:TECHnology[?]`
 * 
 * **Handler:** _scpi_cmd_query_str  
 * **Command ID:** CMD_DUT_TECHNOLOGY
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("DUT:TECHnology[?]");
 * @endcode
 */
        SCPI_COMMAND("DUT:TECHnology[?]", _scpi_cmd_query_str, CMD_DUT_TECHNOLOGY)	\
        /**
 * @brief Set dut:serialnumber[?] value
 * @ingroup scpi_dut_commands
 * 
 * **SCPI Command:** `DUT:SERialnumber[?]`
 * 
 * **Handler:** _scpi_cmd_query_str  
 * **Command ID:** CMD_DUT_SERIAL_NUMBER
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("DUT:SERialnumber[?]");
 * @endcode
 */
        SCPI_COMMAND("DUT:SERialnumber[?]", _scpi_cmd_query_str, CMD_DUT_SERIAL_NUMBER)	\
        /**
 * @brief Set dut:energy[?] value
 * @ingroup scpi_dut_commands
 * 
 * **SCPI Command:** `DUT:ENERGY[?]`
 * 
 * **Handler:** scpi_cmd_rw_float  
 * **Command ID:** CMD_DUT_ENERGY
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("DUT:ENERGY[?]");
 * @endcode
 */
        SCPI_COMMAND("DUT:ENERGY[?]", scpi_cmd_rw_float, CMD_DUT_ENERGY)	\
        /**
 * @brief Set dut:dose[?] value
 * @ingroup scpi_dut_commands
 * 
 * **SCPI Command:** `DUT:DOSE[?]`
 * 
 * **Handler:** scpi_cmd_rw_float  
 * **Command ID:** CMD_DUT_DOSE
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("DUT:DOSE[?]");
 * @endcode
 */
        SCPI_COMMAND("DUT:DOSE[?]", scpi_cmd_rw_float, CMD_DUT_DOSE)	\
        /**
 * @brief Set dut:notes[?] value
 * @ingroup scpi_dut_commands
 * 
 * **SCPI Command:** `DUT:NOTEs[?]`
 * 
 * **Handler:** _scpi_cmd_query_str  
 * **Command ID:** CMD_DUT_NOTES
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("DUT:NOTEs[?]");
 * @endcode
 */
        SCPI_COMMAND("DUT:NOTEs[?]", _scpi_cmd_query_str, CMD_DUT_NOTES)	\
                                                                                                                                    \
        /**
 * @brief Set dut:tsensor:type[?] value
 * @ingroup scpi_dut_commands
 * 
 * **SCPI Command:** `DUT:TSENSor:TYPE[?]`
 * 
 * **Handler:** scpi_cmd_rw_uint8_t  
 * **Command ID:** CMD_DUT_TSENSOR_TYPE
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("DUT:TSENSor:TYPE[?]");
 * @endcode
 */
        SCPI_COMMAND("DUT:TSENSor:TYPE[?]", scpi_cmd_rw_uint8_t, CMD_DUT_TSENSOR_TYPE)	\
        /**
 * @brief Set dut:tsensor:number[?] value
 * @ingroup scpi_dut_commands
 * 
 * **SCPI Command:** `DUT:TSENSor:NUMber[?]`
 * 
 * **Handler:** scpi_cmd_rw_uint8_t  
 * **Command ID:** CMD_DUT_TSENSOR_NUMBER
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("DUT:TSENSor:NUMber[?]");
 * @endcode
 */
        SCPI_COMMAND("DUT:TSENSor:NUMber[?]", scpi_cmd_rw_uint8_t, CMD_DUT_TSENSOR_NUMBER)	\
        /**
 * @brief Set dut:tsensor:fit[?] value
 * @ingroup scpi_dut_commands
 * 
 * **SCPI Command:** `DUT:TSENSor:FIT[?]`
 * 
 * **Handler:** scpi_cmd_rw_amu_coeff_t  
 * **Command ID:** CMD_DUT_TSENSOR_FIT
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("DUT:TSENSor:FIT[?]");
 * @endcode
 */
        SCPI_COMMAND("DUT:TSENSor:FIT[?]", scpi_cmd_rw_amu_coeff_t, CMD_DUT_TSENSOR_FIT)	\
                                                                                                                                    \
        /**
 * @brief Measure all active ADC channels
 * @ingroup scpi_measurement_commands
 * 
 * **SCPI Command:** `MEASure:ADC:ACTive[:RAW]?`
 * 
 * **Handler:** _scpi_cmd_measure_active_ch  
 * **Command ID:** CMD_EXEC_MEAS_ACTIVE_CHANNELS
 * 
 * @param Query only - no parameters
 * @return Numeric value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("MEASure:ADC:ACTive[:RAW]?");
 * @endcode
 */
        SCPI_COMMAND("MEASure:ADC:ACTive[:RAW]?", _scpi_cmd_measure_active_ch, CMD_EXEC_MEAS_ACTIVE_CHANNELS)	\
        /**
 * @brief Measure internal supply voltages
 * @ingroup scpi_measurement_commands
 * 
 * **SCPI Command:** `MEASure:INTERNALvoltages?`
 * 
 * **Handler:** scpi_cmd_rw_amu_int_volt_t  
 * **Command ID:** CMD_EXEC_MEAS_INTERNAL_VOLTAGES
 * 
 * @param Query only - no parameters
 * @return Numeric value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("MEASure:INTERNALvoltages?");
 * @endcode
 */
        SCPI_COMMAND("MEASure:INTERNALvoltages?", scpi_cmd_rw_amu_int_volt_t, CMD_EXEC_MEAS_INTERNAL_VOLTAGES)	\
        /**
 * @brief Measure sun sensor angles
 * @ingroup scpi_measurement_commands
 * 
 * **SCPI Command:** `MEASure:SUNSensor?`
 * 
 * **Handler:** scpi_cmd_rw_ss_angle_t  
 * **Command ID:** CMD_EXEC_MEAS_SUN_SENSOR
 * 
 * @param Query only - no parameters
 * @return Numeric value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("MEASure:SUNSensor?");
 * @endcode
 */
        SCPI_COMMAND("MEASure:SUNSensor?", scpi_cmd_rw_ss_angle_t, CMD_EXEC_MEAS_SUN_SENSOR)	\
        /**
 * @brief Measure pressure sensor data
 * @ingroup scpi_measurement_commands
 * 
 * **SCPI Command:** `MEASure:PRESSure?`
 * 
 * **Handler:** scpi_cmd_rw_press_data_t  
 * **Command ID:** CMD_EXEC_MEAS_PRESSURE_SENSOR
 * 
 * @param Query only - no parameters
 * @return Numeric value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("MEASure:PRESSure?");
 * @endcode
 */
        SCPI_COMMAND("MEASure:PRESSure?", scpi_cmd_rw_press_data_t, CMD_EXEC_MEAS_PRESSURE_SENSOR)	\
                                                                                                                                    \
        /**
 * @brief Perform or query ADC calibration
 * @ingroup scpi_adc_commands
 * 
 * **SCPI Command:** `ADC:CALibrate[?]`
 * 
 * **Handler:** scpi_cmd_exec_qry_float  
 * **Command ID:** CMD_EXEC_ADC_CAL
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("ADC:CALibrate[?]");
 * @endcode
 */
        SCPI_COMMAND("ADC:CALibrate[?]", scpi_cmd_exec_qry_float, CMD_EXEC_ADC_CAL)	\
        /**
 * @brief Calibrate temperature sensor at 25°C
 * @ingroup scpi_adc_commands
 * 
 * **SCPI Command:** `ADC:CALibrate:TSENSor`
 * 
 * **Handler:** scpi_cmd_rw_float  
 * **Command ID:** CMD_EXEC_TSENSOR_CAL_25C
 * 
 * @param Command parameters vary by function
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("ADC:CALibrate:TSENSor");
 * @endcode
 */
        SCPI_COMMAND("ADC:CALibrate:TSENSor", scpi_cmd_rw_float, CMD_EXEC_TSENSOR_CAL_25C)	\
        /**
 * @brief Calibrate all internal ADC channels
 * @ingroup scpi_adc_commands
 * 
 * **SCPI Command:** `ADC:CALibrate:ALL:INTernal`
 * 
 * **Handler:** scpi_cmd_execute  
 * **Command ID:** CMD_EXEC_ADC_CAL_ALL_INTERNAL
 * 
 * @param Command parameters vary by function
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("ADC:CALibrate:ALL:INTernal");
 * @endcode
 */
        SCPI_COMMAND("ADC:CALibrate:ALL:INTernal", scpi_cmd_execute, CMD_EXEC_ADC_CAL_ALL_INTERNAL)	\
        /**
 * @brief Save all internal calibrations to EEPROM
 * @ingroup scpi_adc_commands
 * 
 * **SCPI Command:** `ADC:SAVE:ALL:INTernal`
 * 
 * **Handler:** scpi_cmd_execute  
 * **Command ID:** CMD_EXEC_ADC_CAL_SAVE_ALL_INTERNAL
 * 
 * @param Command parameters vary by function
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("ADC:SAVE:ALL:INTernal");
 * @endcode
 */
        SCPI_COMMAND("ADC:SAVE:ALL:INTernal", scpi_cmd_execute, CMD_EXEC_ADC_CAL_SAVE_ALL_INTERNAL)	\
        /**
 * @brief Set or query active ADC channel mask
 * @ingroup scpi_adc_commands
 * 
 * **SCPI Command:** `ADC:CH:ACTIVE[?]`
 * 
 * **Handler:** scpi_cmd_rw_uint16_t  
 * **Command ID:** AMU_REG_SYSTEM_ADC_ACTIVE_CHANNELS
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("ADC:CH:ACTIVE[?]");
 * @endcode
 */
        SCPI_COMMAND("ADC:CH:ACTIVE[?]", scpi_cmd_rw_uint16_t, AMU_REG_SYSTEM_ADC_ACTIVE_CHANNELS)	\
                                                                                                                                    \
        /**
 * @brief Set or query ADC channel register value
 * @ingroup scpi_adc_commands
 * 
 * **SCPI Command:** `ADC:CH#[?]`
 * 
 * **Handler:** scpi_cmd_rw_uint16_t  
 * **Command ID:** CMD_ADC_CH_REG
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("ADC:CH#[?]");
 * @endcode
 */
        SCPI_COMMAND("ADC:CH#[?]", scpi_cmd_rw_uint16_t, CMD_ADC_CH_REG)	\
        /**
 * @brief Set or query ADC channel setup configuration
 * @ingroup scpi_adc_commands
 * 
 * **SCPI Command:** `ADC:CH#:SETup[?]`
 * 
 * **Handler:** scpi_cmd_rw_uint16_t  
 * **Command ID:** CMD_ADC_CH_SETUP
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("ADC:CH#:SETup[?]");
 * @endcode
 */
        SCPI_COMMAND("ADC:CH#:SETup[?]", scpi_cmd_rw_uint16_t, CMD_ADC_CH_SETUP)	\
        /**
 * @brief Set or query ADC channel filter settings
 * @ingroup scpi_adc_commands
 * 
 * **SCPI Command:** `ADC:CH#:FILTer[?]`
 * 
 * **Handler:** scpi_cmd_rw_uint32_t  
 * **Command ID:** CMD_ADC_CH_FILTER
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("ADC:CH#:FILTer[?]");
 * @endcode
 */
        SCPI_COMMAND("ADC:CH#:FILTer[?]", scpi_cmd_rw_uint32_t, CMD_ADC_CH_FILTER)	\
        /**
 * @brief Set or query ADC channel sample rate
 * @ingroup scpi_adc_commands
 * 
 * **SCPI Command:** `ADC:CH#:RATE[?]`
 * 
 * **Handler:** scpi_cmd_rw_float  
 * **Command ID:** CMD_ADC_CH_RATE
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("ADC:CH#:RATE[?]");
 * @endcode
 */
        SCPI_COMMAND("ADC:CH#:RATE[?]", scpi_cmd_rw_float, CMD_ADC_CH_RATE)	\
        /**
 * @brief Set or query programmable gain amplifier setting
 * @ingroup scpi_adc_commands
 * 
 * **SCPI Command:** `ADC:CH#:PGA[?]`
 * 
 * **Handler:** scpi_cmd_rw_uint8_t  
 * **Command ID:** CMD_ADC_CH_PGA
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("ADC:CH#:PGA[?]");
 * @endcode
 */
        SCPI_COMMAND("ADC:CH#:PGA[?]", scpi_cmd_rw_uint8_t, CMD_ADC_CH_PGA)	\
        /**
 * @brief Query maximum input range for current PGA setting
 * @ingroup scpi_adc_commands
 * 
 * **SCPI Command:** `ADC:CH#:MAX?`
 * 
 * **Handler:** scpi_cmd_rw_float  
 * **Command ID:** CMD_ADC_CH_PGA_MAX
 * 
 * @param Query only - no parameters
 * @return Floating-point value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("ADC:CH#:MAX?");
 * @endcode
 */
        SCPI_COMMAND("ADC:CH#:MAX?", scpi_cmd_rw_float, CMD_ADC_CH_PGA_MAX)	\
        /**
 * @brief Save channel configuration to EEPROM
 * @ingroup scpi_adc_commands
 * 
 * **SCPI Command:** `ADC:CH#:SAVE`
 * 
 * **Handler:** scpi_cmd_execute  
 * **Command ID:** CMD_ADC_CH_SAVE
 * 
 * @param Channel number (0-15)
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("ADC:CH#:SAVE");
 * @endcode
 */
        SCPI_COMMAND("ADC:CH#:SAVE", scpi_cmd_execute, CMD_ADC_CH_SAVE)	\
        /**
 * @brief Set or query channel offset calibration coefficient
 * @ingroup scpi_adc_commands
 * 
 * **SCPI Command:** `ADC:CH#:OFFset[?]`
 * 
 * **Handler:** scpi_cmd_rw_int32_t  
 * **Command ID:** CMD_ADC_CH_OFFSET_COEFF
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("ADC:CH#:OFFset[?]");
 * @endcode
 */
        SCPI_COMMAND("ADC:CH#:OFFset[?]", scpi_cmd_rw_int32_t, CMD_ADC_CH_OFFSET_COEFF)	\
        /**
 * @brief Set or query channel gain calibration coefficient
 * @ingroup scpi_adc_commands
 * 
 * **SCPI Command:** `ADC:CH#:GAIN[?]`
 * 
 * **Handler:** scpi_cmd_rw_int32_t  
 * **Command ID:** CMD_ADC_CH_GAIN_COEFF
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("ADC:CH#:GAIN[?]");
 * @endcode
 */
        SCPI_COMMAND("ADC:CH#:GAIN[?]", scpi_cmd_rw_int32_t, CMD_ADC_CH_GAIN_COEFF)	\
        /**
 * @brief Perform internal calibration for channel
 * @ingroup scpi_adc_commands
 * 
 * **SCPI Command:** `ADC:CH#:CALibrate:INTernal`
 * 
 * **Handler:** scpi_cmd_execute  
 * **Command ID:** CMD_ADC_CH_CAL_INTERNAL
 * 
 * @param Channel number (0-15)
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("ADC:CH#:CALibrate:INTernal");
 * @endcode
 */
        SCPI_COMMAND("ADC:CH#:CALibrate:INTernal", scpi_cmd_execute, CMD_ADC_CH_CAL_INTERNAL)	\
        /**
 * @brief Perform zero-scale calibration for channel
 * @ingroup scpi_adc_commands
 * 
 * **SCPI Command:** `ADC:CH#:CALibrate:ZERO`
 * 
 * **Handler:** scpi_cmd_execute  
 * **Command ID:** CMD_ADC_CH_CAL_ZERO_SCALE
 * 
 * @param Channel number (0-15)
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("ADC:CH#:CALibrate:ZERO");
 * @endcode
 */
        SCPI_COMMAND("ADC:CH#:CALibrate:ZERO", scpi_cmd_execute, CMD_ADC_CH_CAL_ZERO_SCALE)	\
        /**
 * @brief Perform full-scale calibration for channel
 * @ingroup scpi_adc_commands
 * 
 * **SCPI Command:** `ADC:CH#:CALibrate:FULL`
 * 
 * **Handler:** scpi_cmd_execute  
 * **Command ID:** CMD_ADC_CH_CAL_FULL_SCALE
 * 
 * @param Channel number (0-15)
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("ADC:CH#:CALibrate:FULL");
 * @endcode
 */
        SCPI_COMMAND("ADC:CH#:CALibrate:FULL", scpi_cmd_execute, CMD_ADC_CH_CAL_FULL_SCALE)	\
        /**
 * @brief Reset channel calibration to defaults
 * @ingroup scpi_adc_commands
 * 
 * **SCPI Command:** `ADC:CH#:CALibrate:RESet`
 * 
 * **Handler:** scpi_cmd_execute  
 * **Command ID:** CMD_ADC_CH_CAL_RESET
 * 
 * @param Channel number (0-15)
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("ADC:CH#:CALibrate:RESet");
 * @endcode
 */
        SCPI_COMMAND("ADC:CH#:CALibrate:RESet", scpi_cmd_execute, CMD_ADC_CH_CAL_RESET)	\
        /**
 * @brief Save channel calibration to EEPROM
 * @ingroup scpi_adc_commands
 * 
 * **SCPI Command:** `ADC:CH#:CALibrate:SAVe`
 * 
 * **Handler:** scpi_cmd_execute  
 * **Command ID:** CMD_ADC_CH_CAL_SAVE
 * 
 * @param Channel number (0-15)
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("ADC:CH#:CALibrate:SAVe");
 * @endcode
 */
        SCPI_COMMAND("ADC:CH#:CALibrate:SAVe", scpi_cmd_execute, CMD_ADC_CH_CAL_SAVE)	\
                                                                                                                                    \
        /**
 * @brief Query adc:status value
 * @ingroup scpi_adc_commands
 * 
 * **SCPI Command:** `ADC:STATus?`
 * 
 * **Handler:** scpi_cmd_rw_uint32_t  
 * **Command ID:** AMU_REG_SYSTEM_STATUS_HRADC
 * 
 * @param Query only - no parameters
 * @return 32-bit unsigned integer
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("ADC:STATus?");
 * @endcode
 */
        SCPI_COMMAND("ADC:STATus?", scpi_cmd_rw_uint32_t, AMU_REG_SYSTEM_STATUS_HRADC)	\
                                                                                                                                    \
        /**
 * @brief Query sunsensor value
 * @ingroup scpi_sunsensor_commands
 * 
 * **SCPI Command:** `SUNSensor?`
 * 
 * **Handler:** _scpi_read_ptr  
 * **Command ID:** AMU_REG_DATA_PTR_SUNSENSOR
 * 
 * @param Query only - no parameters
 * @return Numeric value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SUNSensor?");
 * @endcode
 */
        SCPI_COMMAND("SUNSensor?", _scpi_read_ptr, AMU_REG_DATA_PTR_SUNSENSOR)	\
                                                                                                                                    \
        /**
 * @brief Query sweep:timestamp value
 * @ingroup scpi_sweep_commands
 * 
 * **SCPI Command:** `SWEEP:TIMEstamp?`
 * 
 * **Handler:** _scpi_read_ptr  
 * **Command ID:** AMU_REG_DATA_PTR_TIMESTAMP
 * 
 * @param Query only - no parameters
 * @return Numeric value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SWEEP:TIMEstamp?");
 * @endcode
 */
        SCPI_COMMAND("SWEEP:TIMEstamp?", _scpi_read_ptr, AMU_REG_DATA_PTR_TIMESTAMP)	\
        /**
 * @brief Query sweep:voltage value
 * @ingroup scpi_sweep_commands
 * 
 * **SCPI Command:** `SWEEP:VOLTage?`
 * 
 * **Handler:** _scpi_read_ptr  
 * **Command ID:** AMU_REG_DATA_PTR_VOLTAGE
 * 
 * @param Query only - no parameters
 * @return Numeric value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SWEEP:VOLTage?");
 * @endcode
 */
        SCPI_COMMAND("SWEEP:VOLTage?", _scpi_read_ptr, AMU_REG_DATA_PTR_VOLTAGE)	\
        /**
 * @brief Query sweep:current value
 * @ingroup scpi_sweep_commands
 * 
 * **SCPI Command:** `SWEEP:CURRent?`
 * 
 * **Handler:** _scpi_read_ptr  
 * **Command ID:** AMU_REG_DATA_PTR_CURRENT
 * 
 * @param Query only - no parameters
 * @return Numeric value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SWEEP:CURRent?");
 * @endcode
 */
        SCPI_COMMAND("SWEEP:CURRent?", _scpi_read_ptr, AMU_REG_DATA_PTR_CURRENT)	\
        /**
 * @brief Query sweep:yaw value
 * @ingroup scpi_sweep_commands
 * 
 * **SCPI Command:** `SWEEP:YAW?`
 * 
 * **Handler:** _scpi_read_ptr  
 * **Command ID:** AMU_REG_DATA_PTR_SS_YAW
 * 
 * @param Query only - no parameters
 * @return Numeric value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SWEEP:YAW?");
 * @endcode
 */
        SCPI_COMMAND("SWEEP:YAW?", _scpi_read_ptr, AMU_REG_DATA_PTR_SS_YAW)	\
        /**
 * @brief Query sweep:pitch value
 * @ingroup scpi_sweep_commands
 * 
 * **SCPI Command:** `SWEEP:PITCH?`
 * 
 * **Handler:** _scpi_read_ptr  
 * **Command ID:** AMU_REG_DATA_PTR_SS_PITCH
 * 
 * @param Query only - no parameters
 * @return Numeric value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SWEEP:PITCH?");
 * @endcode
 */
        SCPI_COMMAND("SWEEP:PITCH?", _scpi_read_ptr, AMU_REG_DATA_PTR_SS_PITCH)	\
                                                                                                                                    \
        /**
 * @brief Set sweep:voltage value
 * @ingroup scpi_sweep_commands
 * 
 * **SCPI Command:** `SWEEP:VOLTage`
 * 
 * **Handler:** _scpi_write_sweep_ptr  
 * **Command ID:** AMU_REG_DATA_PTR_VOLTAGE
 * 
 * @param Command parameters vary by function
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SWEEP:VOLTage");
 * @endcode
 */
        SCPI_COMMAND("SWEEP:VOLTage", _scpi_write_sweep_ptr, AMU_REG_DATA_PTR_VOLTAGE)	\
        /**
 * @brief Set sweep:current value
 * @ingroup scpi_sweep_commands
 * 
 * **SCPI Command:** `SWEEP:CURRent`
 * 
 * **Handler:** _scpi_write_sweep_ptr  
 * **Command ID:** AMU_REG_DATA_PTR_CURRENT
 * 
 * @param Command parameters vary by function
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SWEEP:CURRent");
 * @endcode
 */
        SCPI_COMMAND("SWEEP:CURRent", _scpi_write_sweep_ptr, AMU_REG_DATA_PTR_CURRENT)	\
        /**
 * @brief Set sweep:yaw value
 * @ingroup scpi_sweep_commands
 * 
 * **SCPI Command:** `SWEEP:YAW`
 * 
 * **Handler:** _scpi_write_sweep_ptr  
 * **Command ID:** AMU_REG_DATA_PTR_SS_YAW
 * 
 * @param Command parameters vary by function
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SWEEP:YAW");
 * @endcode
 */
        SCPI_COMMAND("SWEEP:YAW", _scpi_write_sweep_ptr, AMU_REG_DATA_PTR_SS_YAW)	\
        /**
 * @brief Set sweep:pitch value
 * @ingroup scpi_sweep_commands
 * 
 * **SCPI Command:** `SWEEP:PITCH`
 * 
 * **Handler:** _scpi_write_sweep_ptr  
 * **Command ID:** AMU_REG_DATA_PTR_SS_PITCH
 * 
 * @param Command parameters vary by function
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SWEEP:PITCH");
 * @endcode
 */
        SCPI_COMMAND("SWEEP:PITCH", _scpi_write_sweep_ptr, AMU_REG_DATA_PTR_SS_PITCH)	\
                                                                                                                                    \
        /**
 * @brief Set heater:state[?] value
 * @ingroup scpi_heater_commands
 * 
 * **SCPI Command:** `HEATer:STATE[?]`
 * 
 * **Handler:** scpi_cmd_rw_uint8_t  
 * **Command ID:** CMD_AUX_HEATER_STATE
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("HEATer:STATE[?]");
 * @endcode
 */
        SCPI_COMMAND("HEATer:STATE[?]", scpi_cmd_rw_uint8_t, CMD_AUX_HEATER_STATE)	\
        /**
 * @brief Set heater:setpoint[?] value
 * @ingroup scpi_heater_commands
 * 
 * **SCPI Command:** `HEATer:SETpoint[?]`
 * 
 * **Handler:** scpi_cmd_rw_float  
 * **Command ID:** CMD_AUX_HEATER_SETPOINT
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("HEATer:SETpoint[?]");
 * @endcode
 */
        SCPI_COMMAND("HEATer:SETpoint[?]", scpi_cmd_rw_float, CMD_AUX_HEATER_SETPOINT)	\
        /**
 * @brief Set heater:pid[?] value
 * @ingroup scpi_heater_commands
 * 
 * **SCPI Command:** `HEATer:PID[?]`
 * 
 * **Handler:** scpi_cmd_rw_amu_pid_t  
 * **Command ID:** CMD_AUX_HEATER_PID
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("HEATer:PID[?]");
 * @endcode
 */
        SCPI_COMMAND("HEATer:PID[?]", scpi_cmd_rw_amu_pid_t, CMD_AUX_HEATER_PID)	\
        /**
 * @brief Set heater:pid:save value
 * @ingroup scpi_heater_commands
 * 
 * **SCPI Command:** `HEATer:PID:SAVE`
 * 
 * **Handler:** scpi_cmd_execute  
 * **Command ID:** CMD_EXEC_HEATER_PID_SAVE
 * 
 * @param Command parameters vary by function
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("HEATer:PID:SAVE");
 * @endcode
 */
        SCPI_COMMAND("HEATer:PID:SAVE", scpi_cmd_execute, CMD_EXEC_HEATER_PID_SAVE)	\
                                                                                                                                    \
        /**
 * @brief Query sweep:meta value
 * @ingroup scpi_sweep_commands
 * 
 * **SCPI Command:** `SWEEP:META?`
 * 
 * **Handler:** _scpi_read_ptr  
 * **Command ID:** AMU_REG_DATA_PTR_SWEEP_META
 * 
 * @param Query only - no parameters
 * @return Numeric value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SWEEP:META?");
 * @endcode
 */
        SCPI_COMMAND("SWEEP:META?", _scpi_read_ptr, AMU_REG_DATA_PTR_SWEEP_META)	\
        /**
 * @brief Set sweep:meta value
 * @ingroup scpi_sweep_commands
 * 
 * **SCPI Command:** `SWEEP:META`
 * 
 * **Handler:** _scpi_write_meta_ptr  
 * **Command ID:** AMU_REG_DATA_PTR_SWEEP_META
 * 
 * @param Command parameters vary by function
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SWEEP:META");
 * @endcode
 */
        SCPI_COMMAND("SWEEP:META", _scpi_write_meta_ptr, AMU_REG_DATA_PTR_SWEEP_META)	\
                                                                                                                                    \
        /**
 * @brief Query sweep:config value
 * @ingroup scpi_sweep_commands
 * 
 * **SCPI Command:** `SWEEP:CONFig?`
 * 
 * **Handler:** _scpi_read_ptr  
 * **Command ID:** AMU_REG_DATA_PTR_SWEEP_CONFIG
 * 
 * @param Query only - no parameters
 * @return Numeric value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SWEEP:CONFig?");
 * @endcode
 */
        SCPI_COMMAND("SWEEP:CONFig?", _scpi_read_ptr, AMU_REG_DATA_PTR_SWEEP_CONFIG)	\
        /**
 * @brief Set sweep:config value
 * @ingroup scpi_sweep_commands
 * 
 * **SCPI Command:** `SWEEP:CONFig`
 * 
 * **Handler:** _scpi_write_config_ptr  
 * **Command ID:** AMU_REG_DATA_PTR_SWEEP_CONFIG
 * 
 * @param Command parameters vary by function
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SWEEP:CONFig");
 * @endcode
 */
        SCPI_COMMAND("SWEEP:CONFig", _scpi_write_config_ptr, AMU_REG_DATA_PTR_SWEEP_CONFIG)	\
        /**
 * @brief Set sweep:config:save value
 * @ingroup scpi_sweep_commands
 * 
 * **SCPI Command:** `SWEEP:CONFig:SAVe`
 * 
 * **Handler:** scpi_cmd_execute  
 * **Command ID:** CMD_SWEEP_CONFIG_SAVE
 * 
 * @param Command parameters vary by function
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SWEEP:CONFig:SAVe");
 * @endcode
 */
        SCPI_COMMAND("SWEEP:CONFig:SAVe", scpi_cmd_execute, CMD_SWEEP_CONFIG_SAVE)	\
                                                                                                                                    \
        /**
 * @brief Set sweep:disable value
 * @ingroup scpi_sweep_commands
 * 
 * **SCPI Command:** `SWEEP:DISable`
 * 
 * **Handler:** scpi_cmd_execute  
 * **Command ID:** CMD_SWEEP_DISABLE
 * 
 * @param Command parameters vary by function
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SWEEP:DISable");
 * @endcode
 */
        SCPI_COMMAND("SWEEP:DISable", scpi_cmd_execute, CMD_SWEEP_DISABLE)	\
        /**
 * @brief Set sweep:enable value
 * @ingroup scpi_sweep_commands
 * 
 * **SCPI Command:** `SWEEP:ENAble`
 * 
 * **Handler:** scpi_cmd_execute  
 * **Command ID:** CMD_SWEEP_ENABLE
 * 
 * @param Command parameters vary by function
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SWEEP:ENAble");
 * @endcode
 */
        SCPI_COMMAND("SWEEP:ENAble", scpi_cmd_execute, CMD_SWEEP_ENABLE)	\
        /**
 * @brief Set sweep:trigger value
 * @ingroup scpi_sweep_commands
 * 
 * **SCPI Command:** `SWEEP:TRIGger`
 * 
 * **Handler:** scpi_cmd_execute  
 * **Command ID:** CMD_SWEEP_TRIG_SWEEP
 * 
 * @param Command parameters vary by function
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SWEEP:TRIGger");
 * @endcode
 */
        SCPI_COMMAND("SWEEP:TRIGger", scpi_cmd_execute, CMD_SWEEP_TRIG_SWEEP)	\
        /**
 * @brief Set sweep:trigger:initialize value
 * @ingroup scpi_sweep_commands
 * 
 * **SCPI Command:** `SWEEP:TRIGger:INITialize`
 * 
 * **Handler:** scpi_cmd_execute  
 * **Command ID:** CMD_SWEEP_TRIG_INIT
 * 
 * @param Command parameters vary by function
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SWEEP:TRIGger:INITialize");
 * @endcode
 */
        SCPI_COMMAND("SWEEP:TRIGger:INITialize", scpi_cmd_execute, CMD_SWEEP_TRIG_INIT)	\
        /**
 * @brief Query sweep:trigger:isc value
 * @ingroup scpi_sweep_commands
 * 
 * **SCPI Command:** `SWEEP:TRIGger:ISC?`
 * 
 * **Handler:** scpi_cmd_rw_amu_meas_t  
 * **Command ID:** CMD_SWEEP_TRIG_ISC
 * 
 * @param Query only - no parameters
 * @return Numeric value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SWEEP:TRIGger:ISC?");
 * @endcode
 */
        SCPI_COMMAND("SWEEP:TRIGger:ISC?", scpi_cmd_rw_amu_meas_t, CMD_SWEEP_TRIG_ISC)	\
        /**
 * @brief Query sweep:trigger:voc value
 * @ingroup scpi_sweep_commands
 * 
 * **SCPI Command:** `SWEEP:TRIGger:VOC?`
 * 
 * **Handler:** scpi_cmd_rw_amu_meas_t  
 * **Command ID:** CMD_SWEEP_TRIG_VOC
 * 
 * @param Query only - no parameters
 * @return Numeric value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SWEEP:TRIGger:VOC?");
 * @endcode
 */
        SCPI_COMMAND("SWEEP:TRIGger:VOC?", scpi_cmd_rw_amu_meas_t, CMD_SWEEP_TRIG_VOC)	\
        /**
 * @brief Set sweep:trigger:usb value
 * @ingroup scpi_sweep_commands
 * 
 * **SCPI Command:** `SWEEP:TRIGger:USB`
 * 
 * **Handler:** scpi_cmd_execute  
 * **Command ID:** CMD_USB_SWEEP_TRIGGER
 * 
 * @param Command parameters vary by function
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SWEEP:TRIGger:USB");
 * @endcode
 */
        SCPI_COMMAND("SWEEP:TRIGger:USB", scpi_cmd_execute, CMD_USB_SWEEP_TRIGGER)	\
        /**
 * @brief Set sweep:eeprom:save value
 * @ingroup scpi_sweep_commands
 * 
 * **SCPI Command:** `SWEEP:EEPROM:SAVE`
 * 
 * **Handler:** scpi_cmd_execute  
 * **Command ID:** CMD_SWEEP_IV_SAVE_TO_EEPROM
 * 
 * @param Command parameters vary by function
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SWEEP:EEPROM:SAVE");
 * @endcode
 */
        SCPI_COMMAND("SWEEP:EEPROM:SAVE", scpi_cmd_execute, CMD_SWEEP_IV_SAVE_TO_EEPROM)	\
        /**
 * @brief Set sweep:eeprom:load value
 * @ingroup scpi_sweep_commands
 * 
 * **SCPI Command:** `SWEEP:EEPROM:LOAD`
 * 
 * **Handler:** scpi_cmd_execute  
 * **Command ID:** CMD_SWEEP_IV_LOAD_FROM_EEPROM
 * 
 * @param Command parameters vary by function
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SWEEP:EEPROM:LOAD");
 * @endcode
 */
        SCPI_COMMAND("SWEEP:EEPROM:LOAD", scpi_cmd_execute, CMD_SWEEP_IV_LOAD_FROM_EEPROM)	\
        /**
 * @brief Set sweep:aes:encode value
 * @ingroup scpi_sweep_commands
 * 
 * **SCPI Command:** `SWEEP:AES:ENCode`
 * 
 * **Handler:** scpi_cmd_execute  
 * **Command ID:** CMD_SWEEP_AES_ENCODE
 * 
 * @param Command parameters vary by function
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SWEEP:AES:ENCode");
 * @endcode
 */
        SCPI_COMMAND("SWEEP:AES:ENCode", scpi_cmd_execute, CMD_SWEEP_AES_ENCODE)	\
        /**
 * @brief Set sweep:aes:decode value
 * @ingroup scpi_sweep_commands
 * 
 * **SCPI Command:** `SWEEP:AES:DECode`
 * 
 * **Handler:** scpi_cmd_execute  
 * **Command ID:** CMD_SWEEP_AES_DECODE
 * 
 * @param Command parameters vary by function
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SWEEP:AES:DECode");
 * @endcode
 */
        SCPI_COMMAND("SWEEP:AES:DECode", scpi_cmd_execute, CMD_SWEEP_AES_DECODE)	\
        /**
 * @brief Set sweep:datapoint:save value
 * @ingroup scpi_sweep_commands
 * 
 * **SCPI Command:** `SWEEP:DATApoint:SAVE`
 * 
 * **Handler:** scpi_cmd_execute  
 * **Command ID:** CMD_SWEEP_DATAPOINT_SAVE
 * 
 * @param Command parameters vary by function
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SWEEP:DATApoint:SAVE");
 * @endcode
 */
        SCPI_COMMAND("SWEEP:DATApoint:SAVE", scpi_cmd_execute, CMD_SWEEP_DATAPOINT_SAVE)	\
                                                                                                                                    \
        /**
 * @brief Set dac:state[?] value
 * @ingroup scpi_dac_commands
 * 
 * **SCPI Command:** `DAC:STATE[?]`
 * 
 * **Handler:** scpi_cmd_rw_uint8_t  
 * **Command ID:** CMD_AUX_DAC_STATE
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("DAC:STATE[?]");
 * @endcode
 */
        SCPI_COMMAND("DAC:STATE[?]", scpi_cmd_rw_uint8_t, CMD_AUX_DAC_STATE)	\
        /**
 * @brief Set dac:calibrate[?] value
 * @ingroup scpi_dac_commands
 * 
 * **SCPI Command:** `DAC:CALibrate[?]`
 * 
 * **Handler:** scpi_cmd_rw_float  
 * **Command ID:** CMD_EXEC_DAC_CAL
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("DAC:CALibrate[?]");
 * @endcode
 */
        SCPI_COMMAND("DAC:CALibrate[?]", scpi_cmd_rw_float, CMD_EXEC_DAC_CAL)	\
        /**
 * @brief Set dac:calibrate:save value
 * @ingroup scpi_dac_commands
 * 
 * **SCPI Command:** `DAC:CALibrate:SAVe`
 * 
 * **Handler:** scpi_cmd_execute  
 * **Command ID:** CMD_EXEC_DAC_CAL_SAVE
 * 
 * @param Command parameters vary by function
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("DAC:CALibrate:SAVe");
 * @endcode
 */
        SCPI_COMMAND("DAC:CALibrate:SAVe", scpi_cmd_execute, CMD_EXEC_DAC_CAL_SAVE)	\
        /**
 * @brief Set dac:current[?] value
 * @ingroup scpi_dac_commands
 * 
 * **SCPI Command:** `DAC:CURRent[?]`
 * 
 * **Handler:** scpi_cmd_rw_float  
 * **Command ID:** CMD_AUX_DAC_CURRENT
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("DAC:CURRent[?]");
 * @endcode
 */
        SCPI_COMMAND("DAC:CURRent[?]", scpi_cmd_rw_float, CMD_AUX_DAC_CURRENT)	\
        /**
 * @brief Set dac:current:raw[?] value
 * @ingroup scpi_dac_commands
 * 
 * **SCPI Command:** `DAC:CURRent:RAW[?]`
 * 
 * **Handler:** scpi_cmd_rw_uint16_t  
 * **Command ID:** CMD_AUX_DAC_CURRENT_RAW
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("DAC:CURRent:RAW[?]");
 * @endcode
 */
        SCPI_COMMAND("DAC:CURRent:RAW[?]", scpi_cmd_rw_uint16_t, CMD_AUX_DAC_CURRENT_RAW)	\
        /**
 * @brief Set dac:offset[?] value
 * @ingroup scpi_dac_commands
 * 
 * **SCPI Command:** `DAC:OFFset[?]`
 * 
 * **Handler:** scpi_cmd_rw_uint16_t  
 * **Command ID:** CMD_AUX_DAC_OFFSET
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("DAC:OFFset[?]");
 * @endcode
 */
        SCPI_COMMAND("DAC:OFFset[?]", scpi_cmd_rw_uint16_t, CMD_AUX_DAC_OFFSET)	\
        /**
 * @brief Set dac:offset:correction[?] value
 * @ingroup scpi_dac_commands
 * 
 * **SCPI Command:** `DAC:OFFset:CORRection[?]`
 * 
 * **Handler:** scpi_cmd_rw_float  
 * **Command ID:** CMD_AUX_DAC_OFFSET_CORRECTION
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("DAC:OFFset:CORRection[?]");
 * @endcode
 */
        SCPI_COMMAND("DAC:OFFset:CORRection[?]", scpi_cmd_rw_float, CMD_AUX_DAC_OFFSET_CORRECTION)	\
        /**
 * @brief Set dac:gain:correction[?] value
 * @ingroup scpi_dac_commands
 * 
 * **SCPI Command:** `DAC:GAIN:CORRection[?]`
 * 
 * **Handler:** scpi_cmd_rw_float  
 * **Command ID:** CMD_AUX_DAC_GAIN_CORRECTION
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("DAC:GAIN:CORRection[?]");
 * @endcode
 */
        SCPI_COMMAND("DAC:GAIN:CORRection[?]", scpi_cmd_rw_float, CMD_AUX_DAC_GAIN_CORRECTION)	\
        /**
 * @brief Set dac:voltage[?] value
 * @ingroup scpi_dac_commands
 * 
 * **SCPI Command:** `DAC:VOLTage[?]`
 * 
 * **Handler:** scpi_cmd_rw_float  
 * **Command ID:** CMD_AUX_DAC_VOLTAGE
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("DAC:VOLTage[?]");
 * @endcode
 */
        SCPI_COMMAND("DAC:VOLTage[?]", scpi_cmd_rw_float, CMD_AUX_DAC_VOLTAGE)	\
        /**
 * @brief Set dac:voltage:raw[?] value
 * @ingroup scpi_dac_commands
 * 
 * **SCPI Command:** `DAC:VOLTage:RAW[?]`
 * 
 * **Handler:** scpi_cmd_rw_uint16_t  
 * **Command ID:** CMD_AUX_DAC_VOLTAGE_RAW
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("DAC:VOLTage:RAW[?]");
 * @endcode
 */
        SCPI_COMMAND("DAC:VOLTage:RAW[?]", scpi_cmd_rw_uint16_t, CMD_AUX_DAC_VOLTAGE_RAW)	\
                                                                                                                                    \
        /**
 * @brief Set memory:erase:all value
 * @ingroup scpi_memory_commands
 * 
 * **SCPI Command:** `MEMory:ERASE:ALL`
 * 
 * **Handler:** scpi_cmd_execute  
 * **Command ID:** CMD_USB_EEPROM_ERASE_ALL
 * 
 * @param Command parameters vary by function
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("MEMory:ERASE:ALL");
 * @endcode
 */
        SCPI_COMMAND("MEMory:ERASE:ALL", scpi_cmd_execute, CMD_USB_EEPROM_ERASE_ALL)	\
        /**
 * @brief Set memory:erase:config value
 * @ingroup scpi_memory_commands
 * 
 * **SCPI Command:** `MEMory:ERASE:CONFig`
 * 
 * **Handler:** scpi_cmd_execute  
 * **Command ID:** CMD_USB_EEPROM_ERASE_CONFIG
 * 
 * @param Command parameters vary by function
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("MEMory:ERASE:CONFig");
 * @endcode
 */
        SCPI_COMMAND("MEMory:ERASE:CONFig", scpi_cmd_execute, CMD_USB_EEPROM_ERASE_CONFIG)	\
        /**
 * @brief Set memory:adc:ch#:offset[?] value
 * @ingroup scpi_memory_commands
 * 
 * **SCPI Command:** `MEMory:ADC:CH#:OFFset[?]`
 * 
 * **Handler:** scpi_cmd_rw_uint32_t  
 * **Command ID:** CMD_USB_EEPROM_OFFSET
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("MEMory:ADC:CH#:OFFset[?]");
 * @endcode
 */
        SCPI_COMMAND("MEMory:ADC:CH#:OFFset[?]", scpi_cmd_rw_uint32_t, CMD_USB_EEPROM_OFFSET)	\
        /**
 * @brief Set memory:adc:ch#:gain[?] value
 * @ingroup scpi_memory_commands
 * 
 * **SCPI Command:** `MEMory:ADC:CH#:GAIN[?]`
 * 
 * **Handler:** scpi_cmd_rw_uint32_t  
 * **Command ID:** CMD_USB_EEPROM_GAIN
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("MEMory:ADC:CH#:GAIN[?]");
 * @endcode
 */
        SCPI_COMMAND("MEMory:ADC:CH#:GAIN[?]", scpi_cmd_rw_uint32_t, CMD_USB_EEPROM_GAIN)	\
        /**
 * @brief Set memory:voltage:offset#[?] value
 * @ingroup scpi_memory_commands
 * 
 * **SCPI Command:** `MEMory:VOLTage:OFFset#[?]`
 * 
 * **Handler:** scpi_cmd_rw_uint32_t  
 * **Command ID:** CMD_USB_EEPROM_VOLTAGE_OFFSET
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("MEMory:VOLTage:OFFset#[?]");
 * @endcode
 */
        SCPI_COMMAND("MEMory:VOLTage:OFFset#[?]", scpi_cmd_rw_uint32_t, CMD_USB_EEPROM_VOLTAGE_OFFSET)	\
        /**
 * @brief Set memory:current:offset#[?] value
 * @ingroup scpi_memory_commands
 * 
 * **SCPI Command:** `MEMory:CURRent:OFFset#[?]`
 * 
 * **Handler:** scpi_cmd_rw_uint32_t  
 * **Command ID:** CMD_USB_EEPROM_CURRENT_OFFSET
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("MEMory:CURRent:OFFset#[?]");
 * @endcode
 */
        SCPI_COMMAND("MEMory:CURRent:OFFset#[?]", scpi_cmd_rw_uint32_t, CMD_USB_EEPROM_CURRENT_OFFSET)	\
        /**
 * @brief Set memory:voltage:gain#[?] value
 * @ingroup scpi_memory_commands
 * 
 * **SCPI Command:** `MEMory:VOLTage:GAIN#[?]`
 * 
 * **Handler:** scpi_cmd_rw_uint32_t  
 * **Command ID:** CMD_USB_EEPROM_VOLTAGE_GAIN
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("MEMory:VOLTage:GAIN#[?]");
 * @endcode
 */
        SCPI_COMMAND("MEMory:VOLTage:GAIN#[?]", scpi_cmd_rw_uint32_t, CMD_USB_EEPROM_VOLTAGE_GAIN)	\
        /**
 * @brief Set memory:current:gain#[?] value
 * @ingroup scpi_memory_commands
 * 
 * **SCPI Command:** `MEMory:CURRent:GAIN#[?]`
 * 
 * **Handler:** scpi_cmd_rw_uint32_t  
 * **Command ID:** CMD_USB_EEPROM_CURRENT_GAIN
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("MEMory:CURRent:GAIN#[?]");
 * @endcode
 */
        SCPI_COMMAND("MEMory:CURRent:GAIN#[?]", scpi_cmd_rw_uint32_t, CMD_USB_EEPROM_CURRENT_GAIN)

    #define __AMU_EXTENDED_CMD_LIST__																								\
        /**
 * @brief Set adc:voltage:calibrate:zero value
 * @ingroup scpi_adc_commands
 * 
 * **SCPI Command:** `ADC:VOLTage:CALibrate:ZERO`
 * 
 * **Handler:** scpi_cmd_execute  
 * **Command ID:** CMD_USB_ADC_VOLTAGE_CAL_ZERO
 * 
 * @param Command parameters vary by function
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("ADC:VOLTage:CALibrate:ZERO");
 * @endcode
 */
        SCPI_COMMAND("ADC:VOLTage:CALibrate:ZERO", scpi_cmd_execute, CMD_USB_ADC_VOLTAGE_CAL_ZERO)	\
        /**
 * @brief Set adc:voltage:calibrate:full value
 * @ingroup scpi_adc_commands
 * 
 * **SCPI Command:** `ADC:VOLTage:CALibrate:FULL`
 * 
 * **Handler:** scpi_cmd_execute  
 * **Command ID:** CMD_USB_ADC_VOLTAGE_CAL_FULL
 * 
 * @param Command parameters vary by function
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("ADC:VOLTage:CALibrate:FULL");
 * @endcode
 */
        SCPI_COMMAND("ADC:VOLTage:CALibrate:FULL", scpi_cmd_execute, CMD_USB_ADC_VOLTAGE_CAL_FULL)	\
        /**
 * @brief Set adc:voltage:calibrate:reset value
 * @ingroup scpi_adc_commands
 * 
 * **SCPI Command:** `ADC:VOLTage:CALibrate:RESet`
 * 
 * **Handler:** scpi_cmd_execute  
 * **Command ID:** CMD_USB_ADC_VOLTAGE_CAL_RESET
 * 
 * @param Command parameters vary by function
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("ADC:VOLTage:CALibrate:RESet");
 * @endcode
 */
        SCPI_COMMAND("ADC:VOLTage:CALibrate:RESet", scpi_cmd_execute, CMD_USB_ADC_VOLTAGE_CAL_RESET)	\
        /**
 * @brief Set adc:voltage:calibrate:save value
 * @ingroup scpi_adc_commands
 * 
 * **SCPI Command:** `ADC:VOLTage:CALibrate:SAVe`
 * 
 * **Handler:** scpi_cmd_execute  
 * **Command ID:** CMD_USB_ADC_VOLTAGE_CAL_SAVE
 * 
 * @param Command parameters vary by function
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("ADC:VOLTage:CALibrate:SAVe");
 * @endcode
 */
        SCPI_COMMAND("ADC:VOLTage:CALibrate:SAVe", scpi_cmd_execute, CMD_USB_ADC_VOLTAGE_CAL_SAVE)	\
        /**
 * @brief Set adc:voltage:pga[?] value
 * @ingroup scpi_adc_commands
 * 
 * **SCPI Command:** `ADC:VOLTage:PGA[?]`
 * 
 * **Handler:** scpi_cmd_rw_uint8_t  
 * **Command ID:** CMD_USB_ADC_VOLTAGE_PGA
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("ADC:VOLTage:PGA[?]");
 * @endcode
 */
        SCPI_COMMAND("ADC:VOLTage:PGA[?]", scpi_cmd_rw_uint8_t, CMD_USB_ADC_VOLTAGE_PGA)	\
        /**
 * @brief Query adc:voltage:max value
 * @ingroup scpi_adc_commands
 * 
 * **SCPI Command:** `ADC:VOLTage:MAX?`
 * 
 * **Handler:** scpi_cmd_rw_float  
 * **Command ID:** CMD_USB_ADC_VOLTAGE_MAX
 * 
 * @param Query only - no parameters
 * @return Floating-point value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("ADC:VOLTage:MAX?");
 * @endcode
 */
        SCPI_COMMAND("ADC:VOLTage:MAX?", scpi_cmd_rw_float, CMD_USB_ADC_VOLTAGE_MAX)	\
        /**
 * @brief Query adc:voltage:max:pga# value
 * @ingroup scpi_adc_commands
 * 
 * **SCPI Command:** `ADC:VOLTage:MAX:PGA#?`
 * 
 * **Handler:** scpi_cmd_rw_float  
 * **Command ID:** CMD_ADC_CH_PGA_VMAX
 * 
 * @param Query only - no parameters
 * @return Floating-point value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("ADC:VOLTage:MAX:PGA#?");
 * @endcode
 */
        SCPI_COMMAND("ADC:VOLTage:MAX:PGA#?", scpi_cmd_rw_float, CMD_ADC_CH_PGA_VMAX)	\
        /**
 * @brief Set adc:voltage:offset[?] value
 * @ingroup scpi_adc_commands
 * 
 * **SCPI Command:** `ADC:VOLTage:OFFset[?]`
 * 
 * **Handler:** scpi_cmd_rw_int32_t  
 * **Command ID:** CMD_USB_ADC_VOLTAGE_OFFSET
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("ADC:VOLTage:OFFset[?]");
 * @endcode
 */
        SCPI_COMMAND("ADC:VOLTage:OFFset[?]", scpi_cmd_rw_int32_t, CMD_USB_ADC_VOLTAGE_OFFSET)	\
        /**
 * @brief Set adc:voltage:gain[?] value
 * @ingroup scpi_adc_commands
 * 
 * **SCPI Command:** `ADC:VOLTage:GAIN[?]`
 * 
 * **Handler:** scpi_cmd_rw_int32_t  
 * **Command ID:** CMD_USB_ADC_VOLTAGE_GAIN
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("ADC:VOLTage:GAIN[?]");
 * @endcode
 */
        SCPI_COMMAND("ADC:VOLTage:GAIN[?]", scpi_cmd_rw_int32_t, CMD_USB_ADC_VOLTAGE_GAIN)	\
        /**
 * @brief Set adc:current:calibrate:zero value
 * @ingroup scpi_adc_commands
 * 
 * **SCPI Command:** `ADC:CURRent:CALibrate:ZERO`
 * 
 * **Handler:** scpi_cmd_execute  
 * **Command ID:** CMD_USB_ADC_CURRENT_CAL_ZERO
 * 
 * @param Command parameters vary by function
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("ADC:CURRent:CALibrate:ZERO");
 * @endcode
 */
        SCPI_COMMAND("ADC:CURRent:CALibrate:ZERO", scpi_cmd_execute, CMD_USB_ADC_CURRENT_CAL_ZERO)	\
        /**
 * @brief Set adc:current:calibrate:full value
 * @ingroup scpi_adc_commands
 * 
 * **SCPI Command:** `ADC:CURRent:CALibrate:FULL`
 * 
 * **Handler:** scpi_cmd_execute  
 * **Command ID:** CMD_USB_ADC_CURRENT_CAL_FULL
 * 
 * @param Command parameters vary by function
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("ADC:CURRent:CALibrate:FULL");
 * @endcode
 */
        SCPI_COMMAND("ADC:CURRent:CALibrate:FULL", scpi_cmd_execute, CMD_USB_ADC_CURRENT_CAL_FULL)	\
        /**
 * @brief Set adc:current:calibrate:reset value
 * @ingroup scpi_adc_commands
 * 
 * **SCPI Command:** `ADC:CURRent:CALibrate:RESet`
 * 
 * **Handler:** scpi_cmd_execute  
 * **Command ID:** CMD_USB_ADC_CURRENT_CAL_RESET
 * 
 * @param Command parameters vary by function
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("ADC:CURRent:CALibrate:RESet");
 * @endcode
 */
        SCPI_COMMAND("ADC:CURRent:CALibrate:RESet", scpi_cmd_execute, CMD_USB_ADC_CURRENT_CAL_RESET)	\
        /**
 * @brief Set adc:current:calibrate:save value
 * @ingroup scpi_adc_commands
 * 
 * **SCPI Command:** `ADC:CURRent:CALibrate:SAVe`
 * 
 * **Handler:** scpi_cmd_execute  
 * **Command ID:** CMD_USB_ADC_CURRENT_CAL_SAVE
 * 
 * @param Command parameters vary by function
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("ADC:CURRent:CALibrate:SAVe");
 * @endcode
 */
        SCPI_COMMAND("ADC:CURRent:CALibrate:SAVe", scpi_cmd_execute, CMD_USB_ADC_CURRENT_CAL_SAVE)	\
        /**
 * @brief Set adc:current:pga[?] value
 * @ingroup scpi_adc_commands
 * 
 * **SCPI Command:** `ADC:CURRent:PGA[?]`
 * 
 * **Handler:** scpi_cmd_rw_uint8_t  
 * **Command ID:** CMD_USB_ADC_CURRENT_PGA
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("ADC:CURRent:PGA[?]");
 * @endcode
 */
        SCPI_COMMAND("ADC:CURRent:PGA[?]", scpi_cmd_rw_uint8_t, CMD_USB_ADC_CURRENT_PGA)	\
        /**
 * @brief Query adc:current:max value
 * @ingroup scpi_adc_commands
 * 
 * **SCPI Command:** `ADC:CURRent:MAX?`
 * 
 * **Handler:** scpi_cmd_rw_float  
 * **Command ID:** CMD_USB_ADC_CURRENT_MAX
 * 
 * @param Query only - no parameters
 * @return Floating-point value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("ADC:CURRent:MAX?");
 * @endcode
 */
        SCPI_COMMAND("ADC:CURRent:MAX?", scpi_cmd_rw_float, CMD_USB_ADC_CURRENT_MAX)	\
        /**
 * @brief Query adc:current:max:pga# value
 * @ingroup scpi_adc_commands
 * 
 * **SCPI Command:** `ADC:CURRent:MAX:PGA#?`
 * 
 * **Handler:** scpi_cmd_rw_float  
 * **Command ID:** CMD_ADC_CH_PGA_IMAX
 * 
 * @param Query only - no parameters
 * @return Floating-point value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("ADC:CURRent:MAX:PGA#?");
 * @endcode
 */
        SCPI_COMMAND("ADC:CURRent:MAX:PGA#?", scpi_cmd_rw_float, CMD_ADC_CH_PGA_IMAX)	\
        /**
 * @brief Set adc:current:offset[?] value
 * @ingroup scpi_adc_commands
 * 
 * **SCPI Command:** `ADC:CURRent:OFFset[?]`
 * 
 * **Handler:** scpi_cmd_rw_int32_t  
 * **Command ID:** CMD_USB_ADC_CURRENT_OFFSET
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("ADC:CURRent:OFFset[?]");
 * @endcode
 */
        SCPI_COMMAND("ADC:CURRent:OFFset[?]", scpi_cmd_rw_int32_t, CMD_USB_ADC_CURRENT_OFFSET)	\
        /**
 * @brief Set adc:current:gain[?] value
 * @ingroup scpi_adc_commands
 * 
 * **SCPI Command:** `ADC:CURRent:GAIN[?]`
 * 
 * **Handler:** scpi_cmd_rw_int32_t  
 * **Command ID:** CMD_USB_ADC_CURRENT_GAIN
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("ADC:CURRent:GAIN[?]");
 * @endcode
 */
        SCPI_COMMAND("ADC:CURRent:GAIN[?]", scpi_cmd_rw_int32_t, CMD_USB_ADC_CURRENT_GAIN)	\
                                                                                                                                    \
        /**
 * @brief Query sweep:meta:voc value
 * @ingroup scpi_sweep_commands
 * 
 * **SCPI Command:** `SWEEP:META:VOC?`
 * 
 * **Handler:** scpi_cmd_rw_float  
 * **Command ID:** AMU_REG_SWEEP_META_VOC
 * 
 * @param Query only - no parameters
 * @return Floating-point value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SWEEP:META:VOC?");
 * @endcode
 */
        SCPI_COMMAND("SWEEP:META:VOC?", scpi_cmd_rw_float, AMU_REG_SWEEP_META_VOC)	\
        /**
 * @brief Query sweep:meta:isc value
 * @ingroup scpi_sweep_commands
 * 
 * **SCPI Command:** `SWEEP:META:ISC?`
 * 
 * **Handler:** scpi_cmd_rw_float  
 * **Command ID:** AMU_REG_SWEEP_META_ISC
 * 
 * @param Query only - no parameters
 * @return Floating-point value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SWEEP:META:ISC?");
 * @endcode
 */
        SCPI_COMMAND("SWEEP:META:ISC?", scpi_cmd_rw_float, AMU_REG_SWEEP_META_ISC)	\
        /**
 * @brief Query sweep:meta:tsensor:start value
 * @ingroup scpi_sweep_commands
 * 
 * **SCPI Command:** `SWEEP:META:TSENSor:START?`
 * 
 * **Handler:** scpi_cmd_rw_float  
 * **Command ID:** AMU_REG_SWEEP_META_TSENSOR_START
 * 
 * @param Query only - no parameters
 * @return Floating-point value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SWEEP:META:TSENSor:START?");
 * @endcode
 */
        SCPI_COMMAND("SWEEP:META:TSENSor:START?", scpi_cmd_rw_float, AMU_REG_SWEEP_META_TSENSOR_START)	\
        /**
 * @brief Query sweep:meta:tsensor:end value
 * @ingroup scpi_sweep_commands
 * 
 * **SCPI Command:** `SWEEP:META:TSENSor:END?`
 * 
 * **Handler:** scpi_cmd_rw_float  
 * **Command ID:** AMU_REG_SWEEP_META_TSENSOR_END
 * 
 * @param Query only - no parameters
 * @return Floating-point value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SWEEP:META:TSENSor:END?");
 * @endcode
 */
        SCPI_COMMAND("SWEEP:META:TSENSor:END?", scpi_cmd_rw_float, AMU_REG_SWEEP_META_TSENSOR_END)	\
        /**
 * @brief Query sweep:meta:ff value
 * @ingroup scpi_sweep_commands
 * 
 * **SCPI Command:** `SWEEP:META:FF?`
 * 
 * **Handler:** scpi_cmd_rw_float  
 * **Command ID:** AMU_REG_SWEEP_META_FF
 * 
 * @param Query only - no parameters
 * @return Floating-point value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SWEEP:META:FF?");
 * @endcode
 */
        SCPI_COMMAND("SWEEP:META:FF?", scpi_cmd_rw_float, AMU_REG_SWEEP_META_FF)	\
        /**
 * @brief Query sweep:meta:eff value
 * @ingroup scpi_sweep_commands
 * 
 * **SCPI Command:** `SWEEP:META:EFF?`
 * 
 * **Handler:** scpi_cmd_rw_float  
 * **Command ID:** AMU_REG_SWEEP_META_EFF
 * 
 * @param Query only - no parameters
 * @return Floating-point value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SWEEP:META:EFF?");
 * @endcode
 */
        SCPI_COMMAND("SWEEP:META:EFF?", scpi_cmd_rw_float, AMU_REG_SWEEP_META_EFF)	\
        /**
 * @brief Query sweep:meta:vmax value
 * @ingroup scpi_sweep_commands
 * 
 * **SCPI Command:** `SWEEP:META:VMAX?`
 * 
 * **Handler:** scpi_cmd_rw_float  
 * **Command ID:** AMU_REG_SWEEP_META_VMAX
 * 
 * @param Query only - no parameters
 * @return Floating-point value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SWEEP:META:VMAX?");
 * @endcode
 */
        SCPI_COMMAND("SWEEP:META:VMAX?", scpi_cmd_rw_float, AMU_REG_SWEEP_META_VMAX)	\
        /**
 * @brief Query sweep:meta:imax value
 * @ingroup scpi_sweep_commands
 * 
 * **SCPI Command:** `SWEEP:META:IMAX?`
 * 
 * **Handler:** scpi_cmd_rw_float  
 * **Command ID:** AMU_REG_SWEEP_META_IMAX
 * 
 * @param Query only - no parameters
 * @return Floating-point value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SWEEP:META:IMAX?");
 * @endcode
 */
        SCPI_COMMAND("SWEEP:META:IMAX?", scpi_cmd_rw_float, AMU_REG_SWEEP_META_IMAX)	\
        /**
 * @brief Query sweep:meta:pmax value
 * @ingroup scpi_sweep_commands
 * 
 * **SCPI Command:** `SWEEP:META:PMAX?`
 * 
 * **Handler:** scpi_cmd_rw_float  
 * **Command ID:** AMU_REG_SWEEP_META_PMAX
 * 
 * @param Query only - no parameters
 * @return Floating-point value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SWEEP:META:PMAX?");
 * @endcode
 */
        SCPI_COMMAND("SWEEP:META:PMAX?", scpi_cmd_rw_float, AMU_REG_SWEEP_META_PMAX)	\
        /**
 * @brief Query sweep:meta:adc value
 * @ingroup scpi_sweep_commands
 * 
 * **SCPI Command:** `SWEEP:META:ADC?`
 * 
 * **Handler:** scpi_cmd_rw_float  
 * **Command ID:** AMU_REG_SWEEP_META_ADC
 * 
 * @param Query only - no parameters
 * @return Floating-point value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SWEEP:META:ADC?");
 * @endcode
 */
        SCPI_COMMAND("SWEEP:META:ADC?", scpi_cmd_rw_float, AMU_REG_SWEEP_META_ADC)	\
        /**
 * @brief Query sweep:meta:timestamp value
 * @ingroup scpi_sweep_commands
 * 
 * **SCPI Command:** `SWEEP:META:TIMEstamp?`
 * 
 * **Handler:** scpi_cmd_rw_uint32_t  
 * **Command ID:** AMU_REG_SWEEP_META_TIMESTAMP
 * 
 * @param Query only - no parameters
 * @return 32-bit unsigned integer
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SWEEP:META:TIMEstamp?");
 * @endcode
 */
        SCPI_COMMAND("SWEEP:META:TIMEstamp?", scpi_cmd_rw_uint32_t, AMU_REG_SWEEP_META_TIMESTAMP)	\
        /**
 * @brief Query sweep:meta:crc value
 * @ingroup scpi_sweep_commands
 * 
 * **SCPI Command:** `SWEEP:META:CRC?`
 * 
 * **Handler:** scpi_cmd_rw_uint32_t  
 * **Command ID:** AMU_REG_SWEEP_META_CRC
 * 
 * @param Query only - no parameters
 * @return 32-bit unsigned integer
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SWEEP:META:CRC?");
 * @endcode
 */
        SCPI_COMMAND("SWEEP:META:CRC?", scpi_cmd_rw_uint32_t, AMU_REG_SWEEP_META_CRC)	\
                                                                                                                                    \
        /**
 * @brief Set sweep:config:type[?] value
 * @ingroup scpi_sweep_commands
 * 
 * **SCPI Command:** `SWEEP:CONFig:TYPE[?]`
 * 
 * **Handler:** scpi_cmd_rw_uint8_t  
 * **Command ID:** AMU_REG_SWEEP_CONFIG_TYPE
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SWEEP:CONFig:TYPE[?]");
 * @endcode
 */
        SCPI_COMMAND("SWEEP:CONFig:TYPE[?]", scpi_cmd_rw_uint8_t, AMU_REG_SWEEP_CONFIG_TYPE)	\
        /**
 * @brief Set sweep:config:numpoints[?] value
 * @ingroup scpi_sweep_commands
 * 
 * **SCPI Command:** `SWEEP:CONFig:NUMpoints[?]`
 * 
 * **Handler:** scpi_cmd_rw_uint8_t  
 * **Command ID:** AMU_REG_SWEEP_CONFIG_NUM_POINTS
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SWEEP:CONFig:NUMpoints[?]");
 * @endcode
 */
        SCPI_COMMAND("SWEEP:CONFig:NUMpoints[?]", scpi_cmd_rw_uint8_t, AMU_REG_SWEEP_CONFIG_NUM_POINTS)	\
        /**
 * @brief Set sweep:config:delay[?] value
 * @ingroup scpi_sweep_commands
 * 
 * **SCPI Command:** `SWEEP:CONFig:DELAY[?]`
 * 
 * **Handler:** scpi_cmd_rw_uint8_t  
 * **Command ID:** AMU_REG_SWEEP_CONFIG_DELAY
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SWEEP:CONFig:DELAY[?]");
 * @endcode
 */
        SCPI_COMMAND("SWEEP:CONFig:DELAY[?]", scpi_cmd_rw_uint8_t, AMU_REG_SWEEP_CONFIG_DELAY)	\
        /**
 * @brief Set sweep:config:adc:averages[?] value
 * @ingroup scpi_sweep_commands
 * 
 * **SCPI Command:** `SWEEP:CONFig:ADC:AVERages[?]`
 * 
 * **Handler:** scpi_cmd_rw_uint8_t  
 * **Command ID:** AMU_REG_SWEEP_CONFIG_ADC_AVERAGES
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SWEEP:CONFig:ADC:AVERages[?]");
 * @endcode
 */
        SCPI_COMMAND("SWEEP:CONFig:ADC:AVERages[?]", scpi_cmd_rw_uint8_t, AMU_REG_SWEEP_CONFIG_ADC_AVERAGES)	\
        /**
 * @brief Set sweep:config:ratio[?] value
 * @ingroup scpi_sweep_commands
 * 
 * **SCPI Command:** `SWEEP:CONFig:RATIO[?]`
 * 
 * **Handler:** scpi_cmd_rw_uint8_t  
 * **Command ID:** AMU_REG_SWEEP_CONFIG_RATIO
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SWEEP:CONFig:RATIO[?]");
 * @endcode
 */
        SCPI_COMMAND("SWEEP:CONFig:RATIO[?]", scpi_cmd_rw_uint8_t, AMU_REG_SWEEP_CONFIG_RATIO)	\
        /**
 * @brief Set sweep:config:power[?] value
 * @ingroup scpi_sweep_commands
 * 
 * **SCPI Command:** `SWEEP:CONFig:POWER[?]`
 * 
 * **Handler:** scpi_cmd_rw_uint8_t  
 * **Command ID:** AMU_REG_SWEEP_CONFIG_PWR_MODE
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SWEEP:CONFig:POWER[?]");
 * @endcode
 */
        SCPI_COMMAND("SWEEP:CONFig:POWER[?]", scpi_cmd_rw_uint8_t, AMU_REG_SWEEP_CONFIG_PWR_MODE)	\
        /**
 * @brief Set sweep:config:gain[?] value
 * @ingroup scpi_sweep_commands
 * 
 * **SCPI Command:** `SWEEP:CONFig:GAIN[?]`
 * 
 * **Handler:** scpi_cmd_rw_uint8_t  
 * **Command ID:** AMU_REG_SWEEP_CONFIG_DAC_GAIN
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SWEEP:CONFig:GAIN[?]");
 * @endcode
 */
        SCPI_COMMAND("SWEEP:CONFig:GAIN[?]", scpi_cmd_rw_uint8_t, AMU_REG_SWEEP_CONFIG_DAC_GAIN)	\
        /**
 * @brief Set sweep:config:averages[?] value
 * @ingroup scpi_sweep_commands
 * 
 * **SCPI Command:** `SWEEP:CONFig:AVERages[?]`
 * 
 * **Handler:** scpi_cmd_rw_uint8_t  
 * **Command ID:** AMU_REG_SWEEP_CONFIG_AVERAGES
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SWEEP:CONFig:AVERages[?]");
 * @endcode
 */
        SCPI_COMMAND("SWEEP:CONFig:AVERages[?]", scpi_cmd_rw_uint8_t, AMU_REG_SWEEP_CONFIG_AVERAGES)	\
        /**
 * @brief Set sweep:config:am0[?] value
 * @ingroup scpi_sweep_commands
 * 
 * **SCPI Command:** `SWEEP:CONFig:AM0[?]`
 * 
 * **Handler:** scpi_cmd_rw_float  
 * **Command ID:** AMU_REG_SWEEP_CONFIG_AM0
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SWEEP:CONFig:AM0[?]");
 * @endcode
 */
        SCPI_COMMAND("SWEEP:CONFig:AM0[?]", scpi_cmd_rw_float, AMU_REG_SWEEP_CONFIG_AM0)	\
        /**
 * @brief Set sweep:config:area[?] value
 * @ingroup scpi_sweep_commands
 * 
 * **SCPI Command:** `SWEEP:CONFig:AREA[?]`
 * 
 * **Handler:** scpi_cmd_rw_float  
 * **Command ID:** AMU_REG_SWEEP_CONFIG_AREA
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SWEEP:CONFig:AREA[?]");
 * @endcode
 */
        SCPI_COMMAND("SWEEP:CONFig:AREA[?]", scpi_cmd_rw_float, AMU_REG_SWEEP_CONFIG_AREA)	\
                                                                                                                                    \
        /**
 * @brief Query sunsensor:tl value
 * @ingroup scpi_sunsensor_commands
 * 
 * **SCPI Command:** `SUNSensor:TL?`
 * 
 * **Handler:** scpi_cmd_rw_float  
 * **Command ID:** AMU_REG_SUNSENSOR_TL
 * 
 * @param Query only - no parameters
 * @return Floating-point value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SUNSensor:TL?");
 * @endcode
 */
        SCPI_COMMAND("SUNSensor:TL?", scpi_cmd_rw_float, AMU_REG_SUNSENSOR_TL)	\
        /**
 * @brief Query sunsensor:bl value
 * @ingroup scpi_sunsensor_commands
 * 
 * **SCPI Command:** `SUNSensor:BL?`
 * 
 * **Handler:** scpi_cmd_rw_float  
 * **Command ID:** AMU_REG_SUNSENSOR_BL
 * 
 * @param Query only - no parameters
 * @return Floating-point value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SUNSensor:BL?");
 * @endcode
 */
        SCPI_COMMAND("SUNSensor:BL?", scpi_cmd_rw_float, AMU_REG_SUNSENSOR_BL)	\
        /**
 * @brief Query sunsensor:br value
 * @ingroup scpi_sunsensor_commands
 * 
 * **SCPI Command:** `SUNSensor:BR?`
 * 
 * **Handler:** scpi_cmd_rw_float  
 * **Command ID:** AMU_REG_SUNSENSOR_BR
 * 
 * @param Query only - no parameters
 * @return Floating-point value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SUNSensor:BR?");
 * @endcode
 */
        SCPI_COMMAND("SUNSensor:BR?", scpi_cmd_rw_float, AMU_REG_SUNSENSOR_BR)	\
        /**
 * @brief Query sunsensor:tr value
 * @ingroup scpi_sunsensor_commands
 * 
 * **SCPI Command:** `SUNSensor:TR?`
 * 
 * **Handler:** scpi_cmd_rw_float  
 * **Command ID:** AMU_REG_SUNSENSOR_TR
 * 
 * @param Query only - no parameters
 * @return Floating-point value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SUNSensor:TR?");
 * @endcode
 */
        SCPI_COMMAND("SUNSensor:TR?", scpi_cmd_rw_float, AMU_REG_SUNSENSOR_TR)	\
        /**
 * @brief Query sunsensor:yaw value
 * @ingroup scpi_sunsensor_commands
 * 
 * **SCPI Command:** `SUNSensor:YAW?`
 * 
 * **Handler:** scpi_cmd_rw_float  
 * **Command ID:** AMU_REG_SUNSENSOR_YAW
 * 
 * @param Query only - no parameters
 * @return Floating-point value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SUNSensor:YAW?");
 * @endcode
 */
        SCPI_COMMAND("SUNSensor:YAW?", scpi_cmd_rw_float, AMU_REG_SUNSENSOR_YAW)	\
        /**
 * @brief Query sunsensor:pitch value
 * @ingroup scpi_sunsensor_commands
 * 
 * **SCPI Command:** `SUNSensor:PITCH?`
 * 
 * **Handler:** scpi_cmd_rw_float  
 * **Command ID:** AMU_REG_SUNSENSOR_PITCH
 * 
 * @param Query only - no parameters
 * @return Floating-point value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SUNSensor:PITCH?");
 * @endcode
 */
        SCPI_COMMAND("SUNSensor:PITCH?", scpi_cmd_rw_float, AMU_REG_SUNSENSOR_PITCH)	\
        /**
 * @brief Set sunsensor:fit:yaw[?] value
 * @ingroup scpi_sunsensor_commands
 * 
 * **SCPI Command:** `SUNSensor:FIT:YAW[?]`
 * 
 * **Handler:** scpi_cmd_rw_amu_coeff_t  
 * **Command ID:** CMD_AUX_SUNSENSOR_FIT_YAW_COEFF
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SUNSensor:FIT:YAW[?]");
 * @endcode
 */
        SCPI_COMMAND("SUNSensor:FIT:YAW[?]", scpi_cmd_rw_amu_coeff_t, CMD_AUX_SUNSENSOR_FIT_YAW_COEFF)	\
        /**
 * @brief Set sunsensor:fit:pitch[?] value
 * @ingroup scpi_sunsensor_commands
 * 
 * **SCPI Command:** `SUNSensor:FIT:PITCH[?]`
 * 
 * **Handler:** scpi_cmd_rw_amu_coeff_t  
 * **Command ID:** CMD_AUX_SUNSENSOR_FIT_PITCH_COEFF
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SUNSensor:FIT:PITCH[?]");
 * @endcode
 */
        SCPI_COMMAND("SUNSensor:FIT:PITCH[?]", scpi_cmd_rw_amu_coeff_t, CMD_AUX_SUNSENSOR_FIT_PITCH_COEFF)	\
        /**
 * @brief Set sunsensor:fit:save value
 * @ingroup scpi_sunsensor_commands
 * 
 * **SCPI Command:** `SUNSensor:FIT:SAVE`
 * 
 * **Handler:** scpi_cmd_execute  
 * **Command ID:** CMD_EXEC_SUNSENSOR_COEFF_SAVE
 * 
 * @param Command parameters vary by function
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SUNSensor:FIT:SAVE");
 * @endcode
 */
        SCPI_COMMAND("SUNSensor:FIT:SAVE", scpi_cmd_execute, CMD_EXEC_SUNSENSOR_COEFF_SAVE)	\
        /**
 * @brief Set sunsensor:hval[?] value
 * @ingroup scpi_sunsensor_commands
 * 
 * **SCPI Command:** `SUNSensor:HVAL[?]`
 * 
 * **Handler:** scpi_cmd_rw_float  
 * **Command ID:** CMD_AUX_SUNSENSOR_HVAL
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SUNSensor:HVAL[?]");
 * @endcode
 */
        SCPI_COMMAND("SUNSensor:HVAL[?]", scpi_cmd_rw_float, CMD_AUX_SUNSENSOR_HVAL)	\
        /**
 * @brief Set sunsensor:rval[?] value
 * @ingroup scpi_sunsensor_commands
 * 
 * **SCPI Command:** `SUNSensor:RVAL[?]`
 * 
 * **Handler:** scpi_cmd_rw_float  
 * **Command ID:** CMD_AUX_SUNSENSOR_RVAL
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SUNSensor:RVAL[?]");
 * @endcode
 */
        SCPI_COMMAND("SUNSensor:RVAL[?]", scpi_cmd_rw_float, CMD_AUX_SUNSENSOR_RVAL)	\
        /**
 * @brief Set sunsensor:threshold[?] value
 * @ingroup scpi_sunsensor_commands
 * 
 * **SCPI Command:** `SUNSensor:THRESHold[?]`
 * 
 * **Handler:** scpi_cmd_rw_float  
 * **Command ID:** CMD_AUX_SUNSENSOR_THRESHOLD
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SUNSensor:THRESHold[?]");
 * @endcode
 */
        SCPI_COMMAND("SUNSensor:THRESHold[?]", scpi_cmd_rw_float, CMD_AUX_SUNSENSOR_THRESHOLD)	\
                                                                                                                                    \
        /**
 * @brief Query measure:adc:ch#[:raw] value
 * @ingroup scpi_measurement_commands
 * 
 * **SCPI Command:** `MEASure:ADC:CH#[:RAW]?`
 * 
 * **Handler:** _scpi_cmd_measure_channel  
 * **Command ID:** CMD_EXEC_MEAS_CHANNEL
 * 
 * @param Query only - no parameters
 * @return Numeric value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("MEASure:ADC:CH#[:RAW]?");
 * @endcode
 */
        SCPI_COMMAND("MEASure:ADC:CH#[:RAW]?", _scpi_cmd_measure_channel, CMD_EXEC_MEAS_CHANNEL)	\
        /**
 * @brief Query measure:adc:voltage[:raw] value
 * @ingroup scpi_measurement_commands
 * 
 * **SCPI Command:** `MEASure:ADC:VOLTage[:RAW]?`
 * 
 * **Handler:** _scpi_cmd_measure_channel  
 * **Command ID:** CMD_MEAS_CH_VOLTAGE
 * 
 * @param Query only - no parameters
 * @return Numeric value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("MEASure:ADC:VOLTage[:RAW]?");
 * @endcode
 */
        SCPI_COMMAND("MEASure:ADC:VOLTage[:RAW]?", _scpi_cmd_measure_channel, CMD_MEAS_CH_VOLTAGE)	\
        /**
 * @brief Query measure:adc:current[:raw] value
 * @ingroup scpi_measurement_commands
 * 
 * **SCPI Command:** `MEASure:ADC:CURRent[:RAW]?`
 * 
 * **Handler:** _scpi_cmd_measure_channel  
 * **Command ID:** CMD_MEAS_CH_CURRENT
 * 
 * @param Query only - no parameters
 * @return Numeric value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("MEASure:ADC:CURRent[:RAW]?");
 * @endcode
 */
        SCPI_COMMAND("MEASure:ADC:CURRent[:RAW]?", _scpi_cmd_measure_channel, CMD_MEAS_CH_CURRENT)	\
        /**
 * @brief Query measure:adc:tsensor[:raw] value
 * @ingroup scpi_measurement_commands
 * 
 * **SCPI Command:** `MEASure:ADC:TSENSor[:RAW]?`
 * 
 * **Handler:** _scpi_cmd_measure_channel  
 * **Command ID:** CMD_MEAS_CH_TSENSOR
 * 
 * @param Query only - no parameters
 * @return Numeric value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("MEASure:ADC:TSENSor[:RAW]?");
 * @endcode
 */
        SCPI_COMMAND("MEASure:ADC:TSENSor[:RAW]?", _scpi_cmd_measure_channel, CMD_MEAS_CH_TSENSOR)	\
        /**
 * @brief Query measure:adc:tsensor0[:raw] value
 * @ingroup scpi_measurement_commands
 * 
 * **SCPI Command:** `MEASure:ADC:TSENSOR0[:RAW]?`
 * 
 * **Handler:** _scpi_cmd_measure_channel  
 * **Command ID:** CMD_MEAS_CH_TSENSOR_0
 * 
 * @param Query only - no parameters
 * @return Numeric value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("MEASure:ADC:TSENSOR0[:RAW]?");
 * @endcode
 */
        SCPI_COMMAND("MEASure:ADC:TSENSOR0[:RAW]?", _scpi_cmd_measure_channel, CMD_MEAS_CH_TSENSOR_0)	\
        /**
 * @brief Query measure:adc:tsensor1[:raw] value
 * @ingroup scpi_measurement_commands
 * 
 * **SCPI Command:** `MEASure:ADC:TSENSOR1[:RAW]?`
 * 
 * **Handler:** _scpi_cmd_measure_channel  
 * **Command ID:** CMD_MEAS_CH_TSENSOR_1
 * 
 * @param Query only - no parameters
 * @return Numeric value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("MEASure:ADC:TSENSOR1[:RAW]?");
 * @endcode
 */
        SCPI_COMMAND("MEASure:ADC:TSENSOR1[:RAW]?", _scpi_cmd_measure_channel, CMD_MEAS_CH_TSENSOR_1)	\
        /**
 * @brief Query measure:adc:tsensor2[:raw] value
 * @ingroup scpi_measurement_commands
 * 
 * **SCPI Command:** `MEASure:ADC:TSENSOR2[:RAW]?`
 * 
 * **Handler:** _scpi_cmd_measure_channel  
 * **Command ID:** CMD_MEAS_CH_TSENSOR_2
 * 
 * @param Query only - no parameters
 * @return Numeric value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("MEASure:ADC:TSENSOR2[:RAW]?");
 * @endcode
 */
        SCPI_COMMAND("MEASure:ADC:TSENSOR2[:RAW]?", _scpi_cmd_measure_channel, CMD_MEAS_CH_TSENSOR_2)	\
        /**
 * @brief Query measure:adc:tsensors[:raw] value
 * @ingroup scpi_measurement_commands
 * 
 * **SCPI Command:** `MEASure:ADC:TSENSORS[:RAW]?`
 * 
 * **Handler:** _scpi_cmd_measure_tsensors  
 * **Command ID:** CMD_EXEC_MEAS_TSENSORS
 * 
 * @param Query only - no parameters
 * @return Numeric value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("MEASure:ADC:TSENSORS[:RAW]?");
 * @endcode
 */
        SCPI_COMMAND("MEASure:ADC:TSENSORS[:RAW]?", _scpi_cmd_measure_tsensors, CMD_EXEC_MEAS_TSENSORS)	\
        /**
 * @brief Query measure:adc:sstl[:raw] value
 * @ingroup scpi_measurement_commands
 * 
 * **SCPI Command:** `MEASure:ADC:SSTL[:RAW]?`
 * 
 * **Handler:** _scpi_cmd_measure_channel  
 * **Command ID:** CMD_MEAS_CH_SS_TL
 * 
 * @param Query only - no parameters
 * @return Numeric value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("MEASure:ADC:SSTL[:RAW]?");
 * @endcode
 */
        SCPI_COMMAND("MEASure:ADC:SSTL[:RAW]?", _scpi_cmd_measure_channel, CMD_MEAS_CH_SS_TL)	\
        /**
 * @brief Query measure:adc:ssbl[:raw] value
 * @ingroup scpi_measurement_commands
 * 
 * **SCPI Command:** `MEASure:ADC:SSBL[:RAW]?`
 * 
 * **Handler:** _scpi_cmd_measure_channel  
 * **Command ID:** CMD_MEAS_CH_SS_BL
 * 
 * @param Query only - no parameters
 * @return Numeric value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("MEASure:ADC:SSBL[:RAW]?");
 * @endcode
 */
        SCPI_COMMAND("MEASure:ADC:SSBL[:RAW]?", _scpi_cmd_measure_channel, CMD_MEAS_CH_SS_BL)	\
        /**
 * @brief Query measure:adc:ssbr[:raw] value
 * @ingroup scpi_measurement_commands
 * 
 * **SCPI Command:** `MEASure:ADC:SSBR[:RAW]?`
 * 
 * **Handler:** _scpi_cmd_measure_channel  
 * **Command ID:** CMD_MEAS_CH_SS_BR
 * 
 * @param Query only - no parameters
 * @return Numeric value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("MEASure:ADC:SSBR[:RAW]?");
 * @endcode
 */
        SCPI_COMMAND("MEASure:ADC:SSBR[:RAW]?", _scpi_cmd_measure_channel, CMD_MEAS_CH_SS_BR)	\
        /**
 * @brief Query measure:adc:sstr[:raw] value
 * @ingroup scpi_measurement_commands
 * 
 * **SCPI Command:** `MEASure:ADC:SSTR[:RAW]?`
 * 
 * **Handler:** _scpi_cmd_measure_channel  
 * **Command ID:** CMD_MEAS_CH_SS_TR
 * 
 * @param Query only - no parameters
 * @return Numeric value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("MEASure:ADC:SSTR[:RAW]?");
 * @endcode
 */
        SCPI_COMMAND("MEASure:ADC:SSTR[:RAW]?", _scpi_cmd_measure_channel, CMD_MEAS_CH_SS_TR)	\
        /**
 * @brief Query measure:adc:bias[:raw] value
 * @ingroup scpi_measurement_commands
 * 
 * **SCPI Command:** `MEASure:ADC:BIAS[:RAW]?`
 * 
 * **Handler:** _scpi_cmd_measure_channel  
 * **Command ID:** CMD_MEAS_CH_BIAS
 * 
 * @param Query only - no parameters
 * @return Numeric value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("MEASure:ADC:BIAS[:RAW]?");
 * @endcode
 */
        SCPI_COMMAND("MEASure:ADC:BIAS[:RAW]?", _scpi_cmd_measure_channel, CMD_MEAS_CH_BIAS)	\
        /**
 * @brief Query measure:adc:offset[:raw] value
 * @ingroup scpi_measurement_commands
 * 
 * **SCPI Command:** `MEASure:ADC:OFFset[:RAW]?`
 * 
 * **Handler:** _scpi_cmd_measure_channel  
 * **Command ID:** CMD_MEAS_CH_OFFSET
 * 
 * @param Query only - no parameters
 * @return Numeric value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("MEASure:ADC:OFFset[:RAW]?");
 * @endcode
 */
        SCPI_COMMAND("MEASure:ADC:OFFset[:RAW]?", _scpi_cmd_measure_channel, CMD_MEAS_CH_OFFSET)	\
        /**
 * @brief Query measure:adc:temp[:raw] value
 * @ingroup scpi_measurement_commands
 * 
 * **SCPI Command:** `MEASure:ADC:TEMP[:RAW]?`
 * 
 * **Handler:** _scpi_cmd_measure_channel  
 * **Command ID:** CMD_MEAS_CH_TEMP
 * 
 * @param Query only - no parameters
 * @return Numeric value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("MEASure:ADC:TEMP[:RAW]?");
 * @endcode
 */
        SCPI_COMMAND("MEASure:ADC:TEMP[:RAW]?", _scpi_cmd_measure_channel, CMD_MEAS_CH_TEMP)	\
        /**
 * @brief Query measure:adc:avdd[:raw] value
 * @ingroup scpi_measurement_commands
 * 
 * **SCPI Command:** `MEASure:ADC:AVDD[:RAW]?`
 * 
 * **Handler:** _scpi_cmd_measure_channel  
 * **Command ID:** CMD_MEAS_CH_AVDD
 * 
 * @param Query only - no parameters
 * @return Numeric value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("MEASure:ADC:AVDD[:RAW]?");
 * @endcode
 */
        SCPI_COMMAND("MEASure:ADC:AVDD[:RAW]?", _scpi_cmd_measure_channel, CMD_MEAS_CH_AVDD)	\
        /**
 * @brief Query measure:adc:iovdd[:raw] value
 * @ingroup scpi_measurement_commands
 * 
 * **SCPI Command:** `MEASure:ADC:IOVDD[:RAW]?`
 * 
 * **Handler:** _scpi_cmd_measure_channel  
 * **Command ID:** CMD_MEAS_CH_IOVDD
 * 
 * @param Query only - no parameters
 * @return Numeric value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("MEASure:ADC:IOVDD[:RAW]?");
 * @endcode
 */
        SCPI_COMMAND("MEASure:ADC:IOVDD[:RAW]?", _scpi_cmd_measure_channel, CMD_MEAS_CH_IOVDD)	\
        /**
 * @brief Query measure:adc:aldo[:raw] value
 * @ingroup scpi_measurement_commands
 * 
 * **SCPI Command:** `MEASure:ADC:ALDO[:RAW]?`
 * 
 * **Handler:** _scpi_cmd_measure_channel  
 * **Command ID:** CMD_MEAS_CH_ALDO
 * 
 * @param Query only - no parameters
 * @return Numeric value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("MEASure:ADC:ALDO[:RAW]?");
 * @endcode
 */
        SCPI_COMMAND("MEASure:ADC:ALDO[:RAW]?", _scpi_cmd_measure_channel, CMD_MEAS_CH_ALDO)	\
        /**
 * @brief Query measure:adc:dldo[:raw] value
 * @ingroup scpi_measurement_commands
 * 
 * **SCPI Command:** `MEASure:ADC:DLDO[:RAW]?`
 * 
 * **Handler:** _scpi_cmd_measure_channel  
 * **Command ID:** CMD_MEAS_CH_DLDO
 * 
 * @param Query only - no parameters
 * @return Numeric value
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("MEASure:ADC:DLDO[:RAW]?");
 * @endcode
 */
        SCPI_COMMAND("MEASure:ADC:DLDO[:RAW]?", _scpi_cmd_measure_channel, CMD_MEAS_CH_DLDO)	\
                                                                                                                                    \
        /**
 * @brief Set system:config:save value
 * @ingroup scpi_system_commands
 * 
 * **SCPI Command:** `SYSTem:CONFig:SAVE`
 * 
 * **Handler:** scpi_cmd_execute  
 * **Command ID:** CMD_USB_SYSTEM_CONFIG_SAVE
 * 
 * @param Command parameters vary by function
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SYSTem:CONFig:SAVE");
 * @endcode
 */
        SCPI_COMMAND("SYSTem:CONFig:SAVE", scpi_cmd_execute, CMD_USB_SYSTEM_CONFIG_SAVE)	\
        /**
 * @brief Set system:config:current:gain[?] value
 * @ingroup scpi_system_commands
 * 
 * **SCPI Command:** `SYSTem:CONFig:CURRent:GAIN[?]`
 * 
 * **Handler:** scpi_cmd_rw_float  
 * **Command ID:** CMD_USB_SYSTEM_CONFIG_CURR_GAIN
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SYSTem:CONFig:CURRent:GAIN[?]");
 * @endcode
 */
        SCPI_COMMAND("SYSTem:CONFig:CURRent:GAIN[?]", scpi_cmd_rw_float, CMD_USB_SYSTEM_CONFIG_CURR_GAIN)	\
        /**
 * @brief Set system:config:current:rsense[?] value
 * @ingroup scpi_system_commands
 * 
 * **SCPI Command:** `SYSTem:CONFig:CURRent:Rsense[?]`
 * 
 * **Handler:** scpi_cmd_rw_float  
 * **Command ID:** CMD_USB_SYSTEM_CONFIG_CURR_RSENSE
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SYSTem:CONFig:CURRent:Rsense[?]");
 * @endcode
 */
        SCPI_COMMAND("SYSTem:CONFig:CURRent:Rsense[?]", scpi_cmd_rw_float, CMD_USB_SYSTEM_CONFIG_CURR_RSENSE)	\
        /**
 * @brief Set system:config:voltage:r1[?] value
 * @ingroup scpi_system_commands
 * 
 * **SCPI Command:** `SYSTem:CONFig:VOLTage:R1[?]`
 * 
 * **Handler:** scpi_cmd_rw_float  
 * **Command ID:** CMD_USB_SYSTEM_CONFIG_VOLT_R1
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SYSTem:CONFig:VOLTage:R1[?]");
 * @endcode
 */
        SCPI_COMMAND("SYSTem:CONFig:VOLTage:R1[?]", scpi_cmd_rw_float, CMD_USB_SYSTEM_CONFIG_VOLT_R1)	\
        /**
 * @brief Set system:config:voltage:r2[?] value
 * @ingroup scpi_system_commands
 * 
 * **SCPI Command:** `SYSTem:CONFig:VOLTage:R2[?]`
 * 
 * **Handler:** scpi_cmd_rw_float  
 * **Command ID:** CMD_USB_SYSTEM_CONFIG_VOLT_R2
 * 
 * @param Optional query parameter
 * @return Command acknowledgment
 * 
 * @par Example:
 * @code
 * // Send command
 * send_scpi_command("SYSTem:CONFig:VOLTage:R2[?]");
 * @endcode
 */
        SCPI_COMMAND("SYSTem:CONFig:VOLTage:R2[?]", scpi_cmd_rw_float, CMD_USB_SYSTEM_CONFIG_VOLT_R2)   \

#endif
    
#ifdef	__cplusplus
}
#endif

#endif /* SCPI_H_ */
