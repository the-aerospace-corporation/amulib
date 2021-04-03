/**
 * @file scpi.h
 * @brief TODO
 * 
 * TODO: explain what the commands are for
 *
 * @author	CJM28241
 * @date	5/28/2018 7:13:29 PM
 */ 


#ifndef SCPI_H_
#define SCPI_H_

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
	void amu_scpi_list_commands(void);
	void amu_scpi_add_aux_commands(const scpi_command_t* aux_cmd_list);
	scpi_result_t scpi_cmd_execute(scpi_t *context);
	
	
	#define SCPI_CMD_RW_PROTOTYPE(TYPE)	scpi_result_t scpi_cmd_rw_##TYPE(scpi_t *context);
	SCPI_CMD_RW_PROTOTYPE(uint8_t)
	SCPI_CMD_RW_PROTOTYPE(uint16_t)
	SCPI_CMD_RW_PROTOTYPE(uint32_t)
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

#ifdef SCPI_USE_PROGMEM
	#define SCPI_COMMAND(P, C, T) static const char C ## _ ## T ## _pattern[] PROGMEM = P;
#else
	#define SCPI_COMMAND(P, C, T) static const char C ## _ ## T ## _pattern[] = P;
#endif

	#define __AMU_DEFAULT_CMD_LIST__																							\
	SCPI_COMMAND("*CLS",							SCPI_CoreCls,						0									)	\
	SCPI_COMMAND("*ESE?",							SCPI_CoreEseQ,						0									)	\
	SCPI_COMMAND("*ESR?",							SCPI_CoreEsrQ,						0									)	\
	SCPI_COMMAND("*IDN?",							SCPI_CoreIdnQ,						0									)	\
	SCPI_COMMAND("*OPC",							SCPI_CoreOpc,						0									)	\
	SCPI_COMMAND("*OPC?",							SCPI_CoreOpcQ,						0									)	\
	SCPI_COMMAND("*RST",							SCPI_CoreRst,						0									)	\
	SCPI_COMMAND("*SRE",							SCPI_CoreSre,						0									)	\
	SCPI_COMMAND("*SRE?",							SCPI_CoreSreQ,						0									)	\
	SCPI_COMMAND("*STB?",							SCPI_CoreStbQ,						0									)	\
	SCPI_COMMAND("*TST?",							scpi_cmd_execute,					CMD_SYSTEM_NO_CMD					)	\
	SCPI_COMMAND("*WAI",							SCPI_CoreWai,						0									)	\
																																\
	SCPI_COMMAND("STATus:QUEStionable:ENABle",		SCPI_StatusQuestionableEnable,		0									)	\
	SCPI_COMMAND("STATus:QUEStionable:ENABle?",		SCPI_StatusQuestionableEnableQ,		0									)	\
	SCPI_COMMAND("STATus:PRESet",					SCPI_StatusPreset,					0									)	\
																																\
	SCPI_COMMAND("SYSTem:ERRor[:NEXT]?",			SCPI_SystemErrorNextQ,				0									)	\
	SCPI_COMMAND("SYSTem:ERRor:COUNt?",				SCPI_SystemErrorCountQ,				0									)	\
	SCPI_COMMAND("SYSTem:VERSion?",					SCPI_SystemVersionQ,				0									)	\
	SCPI_COMMAND("SYSTem:SCPI:LIST?",				scpi_cmd_execute,					CMD_USB_SYSTEM_LIST_SCPI_COMMANDS	)	\
																																\
	SCPI_COMMAND("SYSTem:BOOTloader",				scpi_cmd_execute,					CMD_USB_SYSTEM_ENTER_BOOTLOADER		)	\
	SCPI_COMMAND("SYSTem:DEBug#?",					scpi_cmd_rw_float,					CMD_USB_SYSTEM_DEBUG				)	\
	SCPI_COMMAND("SYSTem:FIRMware?",				_scpi_cmd_query_str,				CMD_SYSTEM_FIRMWARE					)	\
	SCPI_COMMAND("SYSTem:HARDware?",				scpi_cmd_rw_uint8_t,				AMU_REG_SYSTEM_HARDWARE_REVISION	)	\
	SCPI_COMMAND("SYSTem:LED:PAT",					_scpi_cmd_led,						CMD_SYSTEM_LED						)	\
	SCPI_COMMAND("SYSTem:LED:COLOR[?]",				scpi_cmd_rw_amu_pid_t,				CMD_SYSTEM_LED_COLOR				)	\
	SCPI_COMMAND("SYSTem:SERial?",					_scpi_cmd_query_str,				CMD_SYSTEM_SERIAL_NUM				)	\
	SCPI_COMMAND("SYSTem:STATus?",					scpi_cmd_rw_uint8_t,				AMU_REG_SYSTEM_AMU_STATUS			)	\
	SCPI_COMMAND("SYSTem:TEMPerature?",				scpi_cmd_rw_float,					CDM_SYSTEM_TEMPERATURE				)	\
	SCPI_COMMAND("SYSTem:TIMEstamp[?]",				scpi_cmd_rw_uint32_t,				CMD_SYSTEM_TIME						)	\
	SCPI_COMMAND("SYSTem:TIMEstamp:UTC[?]",			scpi_cmd_rw_uint32_t,				CMD_SYSTEM_UTC_TIME					)	\
	SCPI_COMMAND("SYSTem:TWI:ADDress[?]",			scpi_cmd_rw_uint8_t,				CMD_SYSTEM_TWI_ADDRESS				)	\
	SCPI_COMMAND("SYSTem:TWI:NUMdevices?",			scpi_cmd_rw_uint8_t,				CMD_SYSTEM_TWI_NUM_DEVICES			)	\
	SCPI_COMMAND("SYSTem:TWI:MODE[?]",				scpi_cmd_rw_uint8_t,				CMD_USB_SYSTEM_TWI_MODE				)	\
	SCPI_COMMAND("SYSTem:TWI:SCAN?",				_scpi_cmd_twi_scan,					CMD_USB_SYSTEM_TWI_SCAN				)	\
	SCPI_COMMAND("SYSTem:TWI:STATus?",				scpi_cmd_rw_uint8_t,				CMD_SYSTEM_TWI_STATUS				)	\
	SCPI_COMMAND("SYSTem:XMEGA:FUSES?",				scpi_cmd_rw_uint32_t,				CMD_SYSTEM_XMEGA_FUSES				)	\
	SCPI_COMMAND("SYSTem:XMEGA:SIGnature?",			_scpi_cmd_query_str,				CMD_SYSTEM_XMEGA_USER_SIGNATURES	)	\
																																\
	SCPI_COMMAND("DUT:JUNCtion[?]",					scpi_cmd_rw_uint8_t,				CMD_DUT_JUNCTION					)	\
	SCPI_COMMAND("DUT:COVERglass[?]",				scpi_cmd_rw_uint8_t,				CMD_DUT_COVERGLASS					)	\
	SCPI_COMMAND("DUT:INTERconnect[?]",				scpi_cmd_rw_uint8_t,				CMD_DUT_INTERCONNECT				)	\
	SCPI_COMMAND("DUT:MANufacturer[?]",				_scpi_cmd_query_str,				CMD_DUT_MANUFACTURER				)	\
	SCPI_COMMAND("DUT:MODel[?]",					_scpi_cmd_query_str,				CMD_DUT_MODEL						)	\
	SCPI_COMMAND("DUT:TECHnology[?]",				_scpi_cmd_query_str,				CMD_DUT_TECHNOLOGY					)	\
	SCPI_COMMAND("DUT:SERialnumber[?]",				_scpi_cmd_query_str,				CMD_DUT_SERIAL_NUMBER				)	\
	SCPI_COMMAND("DUT:ENERGY[?]",					scpi_cmd_rw_float,					CMD_DUT_ENERGY						)	\
	SCPI_COMMAND("DUT:DOSE[?]",						scpi_cmd_rw_float,					CMD_DUT_DOSE						)	\
	SCPI_COMMAND("DUT:NOTEs[?]",					_scpi_cmd_query_str,				CMD_DUT_NOTES						)	\
																																\
	SCPI_COMMAND("DUT:TSENSor:TYPE[?]",				scpi_cmd_rw_uint8_t,				CMD_DUT_TSENSOR_TYPE				)	\
	SCPI_COMMAND("DUT:TSENSor:NUMber[?]",			scpi_cmd_rw_uint8_t,				CMD_DUT_TSENSOR_NUMBER				)	\
	SCPI_COMMAND("DUT:TSENSor:FIT[?]",				scpi_cmd_rw_amu_coeff_t,			CMD_DUT_TSENSOR_FIT					)	\
																																\
	SCPI_COMMAND("MEASure:ADC:ACTive[:RAW]?",		_scpi_cmd_measure_active_ch,		CMD_EXEC_MEAS_ACTIVE_CHANNELS		)	\
	SCPI_COMMAND("MEASure:INTERNALvoltages?",		scpi_cmd_rw_amu_int_volt_t,			CMD_EXEC_MEAS_INTERNAL_VOLTAGES		)	\
	SCPI_COMMAND("MEASure:SUNSensor?",				scpi_cmd_rw_ss_angle_t,				CMD_EXEC_MEAS_SUN_SENSOR			)	\
	SCPI_COMMAND("MEASure:PRESSure?",				scpi_cmd_rw_press_data_t,			CMD_EXEC_MEAS_PRESSURE_SENSOR		)	\
																																\
	SCPI_COMMAND("ADC:CALibrate[?]",				scpi_cmd_exec_qry_float,			CMD_EXEC_ADC_CAL					)	\
	SCPI_COMMAND("ADC:CALibrate:TSENSor",			scpi_cmd_rw_float,					CMD_EXEC_TSENSOR_CAL_25C			)	\
	SCPI_COMMAND("ADC:CALibrate:ALL:INTernal",		scpi_cmd_execute,					CMD_EXEC_ADC_CAL_ALL_INTERNAL		)	\
	SCPI_COMMAND("ADC:SAVE:ALL:INTernal",			scpi_cmd_execute,					CMD_EXEC_ADC_CAL_SAVE_ALL_INTERNAL	)	\
	SCPI_COMMAND("ADC:CH:ACTIVE[?]",				scpi_cmd_rw_uint16_t,				AMU_REG_SYSTEM_ADC_ACTIVE_CHANNELS	)	\
																																\
	SCPI_COMMAND("ADC:CH#[?]",						scpi_cmd_rw_uint16_t,				CMD_ADC_CH_REG						)	\
	SCPI_COMMAND("ADC:CH#:SETup[?]",				scpi_cmd_rw_uint16_t,				CMD_ADC_CH_SETUP					)	\
	SCPI_COMMAND("ADC:CH#:FILTer[?]",				scpi_cmd_rw_uint32_t,				CMD_ADC_CH_FILTER					)	\
	SCPI_COMMAND("ADC:CH#:RATE[?]",					scpi_cmd_rw_float,					CMD_ADC_CH_RATE						)	\
	SCPI_COMMAND("ADC:CH#:PGA[?]",					scpi_cmd_rw_uint8_t,				CMD_ADC_CH_PGA						)	\
	SCPI_COMMAND("ADC:CH#:MAX?",					scpi_cmd_rw_float,					CMD_ADC_CH_PGA_MAX					)	\
	SCPI_COMMAND("ADC:CH#:SAVE",					scpi_cmd_execute,					CMD_ADC_CH_SAVE						)	\
	SCPI_COMMAND("ADC:CH#:OFFset[?]",				scpi_cmd_rw_uint32_t,				CMD_ADC_CH_OFFSET_COEFF				)	\
	SCPI_COMMAND("ADC:CH#:GAIN[?]",					scpi_cmd_rw_uint32_t,				CMD_ADC_CH_GAIN_COEFF				)	\
	SCPI_COMMAND("ADC:CH#:CALibrate:INTernal",		scpi_cmd_execute,					CMD_ADC_CH_CAL_INTERNAL				)	\
	SCPI_COMMAND("ADC:CH#:CALibrate:ZERO"	,		scpi_cmd_execute,					CMD_ADC_CH_CAL_ZERO_SCALE			)	\
	SCPI_COMMAND("ADC:CH#:CALibrate:FULL",			scpi_cmd_execute,					CMD_ADC_CH_CAL_FULL_SCALE			)	\
	SCPI_COMMAND("ADC:CH#:CALibrate:RESet",			scpi_cmd_execute,					CMD_ADC_CH_CAL_RESET				)	\
	SCPI_COMMAND("ADC:CH#:CALibrate:SAVe",			scpi_cmd_execute,					CMD_ADC_CH_CAL_SAVE					)	\
																																\
	SCPI_COMMAND("ADC:STATus?",						scpi_cmd_rw_uint32_t,				AMU_REG_SYSTEM_STATUS_HRADC			)	\
																																\
	SCPI_COMMAND("SUNSensor?",						_scpi_read_ptr,						AMU_REG_DATA_PTR_SUNSENSOR			)	\
																																\
	SCPI_COMMAND("SWEEP:TIMEstamp?",				_scpi_read_ptr,						AMU_REG_DATA_PTR_TIMESTAMP			)	\
	SCPI_COMMAND("SWEEP:VOLTage?",					_scpi_read_ptr,						AMU_REG_DATA_PTR_VOLTAGE			)	\
	SCPI_COMMAND("SWEEP:CURRent?",					_scpi_read_ptr,						AMU_REG_DATA_PTR_CURRENT			)	\
	SCPI_COMMAND("SWEEP:YAW?",						_scpi_read_ptr,						AMU_REG_DATA_PTR_SS_YAW				)	\
	SCPI_COMMAND("SWEEP:PITCH?",					_scpi_read_ptr,						AMU_REG_DATA_PTR_SS_PITCH			)	\
																																\
	SCPI_COMMAND("SWEEP:VOLTage",					_scpi_write_sweep_ptr,				AMU_REG_DATA_PTR_VOLTAGE			)	\
	SCPI_COMMAND("SWEEP:CURRent",					_scpi_write_sweep_ptr,				AMU_REG_DATA_PTR_CURRENT			)	\
	SCPI_COMMAND("SWEEP:YAW",						_scpi_write_sweep_ptr,				AMU_REG_DATA_PTR_SS_YAW				)	\
	SCPI_COMMAND("SWEEP:PITCH",						_scpi_write_sweep_ptr,				AMU_REG_DATA_PTR_SS_PITCH			)	\
																																\
	SCPI_COMMAND("HEATer:STATE[?]",					scpi_cmd_rw_uint8_t,				CMD_AUX_HEATER_STATE				)	\
	SCPI_COMMAND("HEATer:SETpoint[?]",				scpi_cmd_rw_float,					CMD_AUX_HEATER_SETPOINT				)	\
	SCPI_COMMAND("HEATer:PID[?]",					scpi_cmd_rw_amu_pid_t,				CMD_AUX_HEATER_PID					)	\
	SCPI_COMMAND("HEATer:PID:SAVE",					scpi_cmd_execute,					CMD_EXEC_HEATER_PID_SAVE			)	\
																																\
	SCPI_COMMAND("SWEEP:META?",						_scpi_read_ptr,						AMU_REG_DATA_PTR_SWEEP_META			)	\
	SCPI_COMMAND("SWEEP:META",						_scpi_write_meta_ptr,				AMU_REG_DATA_PTR_SWEEP_META			)	\
																																\
	SCPI_COMMAND("SWEEP:CONFig?",					_scpi_read_ptr,						AMU_REG_DATA_PTR_SWEEP_CONFIG		)	\
	SCPI_COMMAND("SWEEP:CONFig",					_scpi_write_config_ptr,				AMU_REG_DATA_PTR_SWEEP_CONFIG		)	\
	SCPI_COMMAND("SWEEP:CONFig:SAVe",				scpi_cmd_execute,					CMD_SWEEP_CONFIG_SAVE				)	\
																																\
	SCPI_COMMAND("SWEEP:DISable",					scpi_cmd_execute,					CMD_SWEEP_DISABLE					)	\
	SCPI_COMMAND("SWEEP:ENAble",					scpi_cmd_execute,					CMD_SWEEP_ENABLE					)	\
	SCPI_COMMAND("SWEEP:TRIGger",					scpi_cmd_execute,					CMD_SWEEP_TRIG_SWEEP				)	\
	SCPI_COMMAND("SWEEP:TRIGger:INITialize",		scpi_cmd_execute,					CMD_SWEEP_TRIG_INIT					)	\
	SCPI_COMMAND("SWEEP:TRIGger:ISC?",				scpi_cmd_rw_amu_meas_t,				CMD_SWEEP_TRIG_ISC					)	\
	SCPI_COMMAND("SWEEP:TRIGger:VOC?",				scpi_cmd_rw_amu_meas_t,				CMD_SWEEP_TRIG_VOC					)	\
	SCPI_COMMAND("SWEEP:TRIGger:USB",				scpi_cmd_execute,					CMD_USB_SWEEP_TRIGGER				)	\
	SCPI_COMMAND("SWEEP:EEPROM:SAVE",				scpi_cmd_execute,					CMD_SWEEP_IV_SAVE_TO_EEPROM			)	\
	SCPI_COMMAND("SWEEP:EEPROM:LOAD",				scpi_cmd_execute,					CMD_SWEEP_IV_LOAD_FROM_EEPROM		)	\
	SCPI_COMMAND("SWEEP:AES:ENCode",				scpi_cmd_execute,					CMD_SWEEP_AES_ENCODE				)	\
	SCPI_COMMAND("SWEEP:AES:DECode",				scpi_cmd_execute,					CMD_SWEEP_AES_DECODE				)	\
	SCPI_COMMAND("SWEEP:DATApoint:SAVE",			scpi_cmd_execute,					CMD_SWEEP_DATAPOINT_SAVE			)	\
																																\
	SCPI_COMMAND("DAC:STATE[?]",					scpi_cmd_rw_uint8_t,				CMD_AUX_DAC_STATE					)	\
	SCPI_COMMAND("DAC:CALibrate[?]",				scpi_cmd_rw_float,					CMD_EXEC_DAC_CAL					)	\
	SCPI_COMMAND("DAC:CALibrate:SAVe",				scpi_cmd_execute,					CMD_EXEC_DAC_CAL_SAVE				)	\
	SCPI_COMMAND("DAC:CURRent[?]",					scpi_cmd_rw_float,					CMD_AUX_DAC_CURRENT					)	\
	SCPI_COMMAND("DAC:CURRent:RAW[?]",				scpi_cmd_rw_uint16_t,				CMD_AUX_DAC_CURRENT_RAW				)	\
	SCPI_COMMAND("DAC:OFFset[?]",					scpi_cmd_rw_uint16_t,				CMD_AUX_DAC_OFFSET					)	\
	SCPI_COMMAND("DAC:OFFset:CORRection[?]",		scpi_cmd_rw_float,					CMD_AUX_DAC_OFFSET_CORRECTION		)	\
	SCPI_COMMAND("DAC:GAIN:CORRection[?]",			scpi_cmd_rw_float,					CMD_AUX_DAC_GAIN_CORRECTION			)	\
	SCPI_COMMAND("DAC:VOLTage[?]",					scpi_cmd_rw_float,					CMD_AUX_DAC_VOLTAGE					)	\
	SCPI_COMMAND("DAC:VOLTage:RAW[?]",				scpi_cmd_rw_uint16_t,				CMD_AUX_DAC_VOLTAGE_RAW				)	\
																																\
	SCPI_COMMAND("MEMory:ERASE:ALL",				scpi_cmd_execute,					CMD_USB_EEPROM_ERASE_ALL			)	\
	SCPI_COMMAND("MEMory:ERASE:CONFig",				scpi_cmd_execute,					CMD_USB_EEPROM_ERASE_CONFIG			)	\
	SCPI_COMMAND("MEMory:ADC:CH#:OFFset[?]",		scpi_cmd_rw_uint32_t,				CMD_USB_EEPROM_OFFSET				)	\
	SCPI_COMMAND("MEMory:ADC:CH#:GAIN[?]",			scpi_cmd_rw_uint32_t,				CMD_USB_EEPROM_GAIN					)	\
	SCPI_COMMAND("MEMory:VOLTage:OFFset#[?]",		scpi_cmd_rw_uint32_t,				CMD_USB_EEPROM_VOLTAGE_OFFSET		)	\
	SCPI_COMMAND("MEMory:CURRent:OFFset#[?]",		scpi_cmd_rw_uint32_t,				CMD_USB_EEPROM_CURRENT_OFFSET		)	\
	SCPI_COMMAND("MEMory:VOLTage:GAIN#[?]",			scpi_cmd_rw_uint32_t,				CMD_USB_EEPROM_VOLTAGE_GAIN			)	\
	SCPI_COMMAND("MEMory:CURRent:GAIN#[?]",			scpi_cmd_rw_uint32_t,				CMD_USB_EEPROM_CURRENT_GAIN			)

#define __AMU_EXTENDED_CMD_LIST__																								\
	SCPI_COMMAND("ADC:VOLTage:CALibrate:ZERO",		scpi_cmd_execute,					CMD_USB_ADC_VOLTAGE_CAL_ZERO		)	\
	SCPI_COMMAND("ADC:VOLTage:CALibrate:FULL",		scpi_cmd_execute,					CMD_USB_ADC_VOLTAGE_CAL_FULL		)	\
	SCPI_COMMAND("ADC:VOLTage:CALibrate:RESet",		scpi_cmd_execute,					CMD_USB_ADC_VOLTAGE_CAL_RESET		)	\
	SCPI_COMMAND("ADC:VOLTage:CALibrate:SAVe",		scpi_cmd_execute,					CMD_USB_ADC_VOLTAGE_CAL_SAVE		)	\
	SCPI_COMMAND("ADC:VOLTage:PGA[?]",				scpi_cmd_rw_uint8_t,				CMD_USB_ADC_VOLTAGE_PGA				)	\
	SCPI_COMMAND("ADC:VOLTage:MAX?",				scpi_cmd_rw_float,					CMD_USB_ADC_VOLTAGE_MAX				)	\
	SCPI_COMMAND("ADC:VOLTage:MAX:PGA#?",			scpi_cmd_rw_float,					CMD_ADC_CH_PGA_VMAX					)	\
	SCPI_COMMAND("ADC:VOLTage:OFFset[?]",			scpi_cmd_rw_uint32_t,				CMD_USB_ADC_VOLTAGE_OFFSET			)	\
	SCPI_COMMAND("ADC:VOLTage:GAIN[?]",				scpi_cmd_rw_uint32_t,				CMD_USB_ADC_VOLTAGE_GAIN			)	\
	SCPI_COMMAND("ADC:CURRent:CALibrate:ZERO",		scpi_cmd_execute,					CMD_USB_ADC_CURRENT_CAL_ZERO		)	\
	SCPI_COMMAND("ADC:CURRent:CALibrate:FULL",		scpi_cmd_execute,					CMD_USB_ADC_CURRENT_CAL_FULL		)	\
	SCPI_COMMAND("ADC:CURRent:CALibrate:RESet",		scpi_cmd_execute,					CMD_USB_ADC_CURRENT_CAL_RESET		)	\
	SCPI_COMMAND("ADC:CURRent:CALibrate:SAVe",		scpi_cmd_execute,					CMD_USB_ADC_CURRENT_CAL_SAVE		)	\
	SCPI_COMMAND("ADC:CURRent:PGA[?]",				scpi_cmd_rw_uint8_t,				CMD_USB_ADC_CURRENT_PGA				)	\
	SCPI_COMMAND("ADC:CURRent:MAX?",				scpi_cmd_rw_float,					CMD_USB_ADC_CURRENT_MAX				)	\
	SCPI_COMMAND("ADC:CURRent:MAX:PGA#?",			scpi_cmd_rw_float,					CMD_ADC_CH_PGA_IMAX					)	\
	SCPI_COMMAND("ADC:CURRent:OFFset[?]",			scpi_cmd_rw_uint32_t,				CMD_USB_ADC_CURRENT_OFFSET			)	\
	SCPI_COMMAND("ADC:CURRent:GAIN[?]",				scpi_cmd_rw_uint32_t,				CMD_USB_ADC_CURRENT_GAIN			)	\
																																\
	SCPI_COMMAND("SWEEP:META:VOC?",					scpi_cmd_rw_float,					AMU_REG_SWEEP_META_VOC				)	\
	SCPI_COMMAND("SWEEP:META:ISC?",					scpi_cmd_rw_float,					AMU_REG_SWEEP_META_ISC				)	\
	SCPI_COMMAND("SWEEP:META:TSENSor:START?",		scpi_cmd_rw_float,					AMU_REG_SWEEP_META_TSENSOR_START	)	\
	SCPI_COMMAND("SWEEP:META:TSENSor:END?",			scpi_cmd_rw_float,					AMU_REG_SWEEP_META_TSENSOR_END		)	\
	SCPI_COMMAND("SWEEP:META:FF?",					scpi_cmd_rw_float,					AMU_REG_SWEEP_META_FF				)	\
	SCPI_COMMAND("SWEEP:META:EFF?",					scpi_cmd_rw_float,					AMU_REG_SWEEP_META_EFF				)	\
	SCPI_COMMAND("SWEEP:META:VMAX?",				scpi_cmd_rw_float,					AMU_REG_SWEEP_META_VMAX				)	\
	SCPI_COMMAND("SWEEP:META:IMAX?",				scpi_cmd_rw_float,					AMU_REG_SWEEP_META_IMAX				)	\
	SCPI_COMMAND("SWEEP:META:PMAX?",				scpi_cmd_rw_float,					AMU_REG_SWEEP_META_PMAX				)	\
	SCPI_COMMAND("SWEEP:META:ADC?",					scpi_cmd_rw_float,					AMU_REG_SWEEP_META_ADC				)	\
	SCPI_COMMAND("SWEEP:META:TIMEstamp?",			scpi_cmd_rw_uint32_t,				AMU_REG_SWEEP_META_TIMESTAMP		)	\
	SCPI_COMMAND("SWEEP:META:CRC?",					scpi_cmd_rw_uint32_t,				AMU_REG_SWEEP_META_CRC				)	\
																																\
	SCPI_COMMAND("SWEEP:CONFig:TYPE[?]",			scpi_cmd_rw_uint8_t,				AMU_REG_SWEEP_CONFIG_TYPE			)	\
	SCPI_COMMAND("SWEEP:CONFig:NUMpoints[?]",		scpi_cmd_rw_uint8_t,				AMU_REG_SWEEP_CONFIG_NUM_POINTS		)	\
	SCPI_COMMAND("SWEEP:CONFig:DELAY[?]",			scpi_cmd_rw_uint8_t,				AMU_REG_SWEEP_CONFIG_DELAY			)	\
	SCPI_COMMAND("SWEEP:CONFig:RATIO[?]",			scpi_cmd_rw_uint8_t,				AMU_REG_SWEEP_CONFIG_RATIO			)	\
	SCPI_COMMAND("SWEEP:CONFig:POWER[?]",			scpi_cmd_rw_uint8_t,				AMU_REG_SWEEP_CONFIG_PWR_MODE		)	\
	SCPI_COMMAND("SWEEP:CONFig:GAIN[?]"	,			scpi_cmd_rw_uint8_t,				AMU_REG_SWEEP_CONFIG_DAC_GAIN		)	\
	SCPI_COMMAND("SWEEP:CONFig:AVERages[?]"	,		scpi_cmd_rw_uint8_t,				AMU_REG_SWEEP_CONFIG_AVERAGES		)	\
	SCPI_COMMAND("SWEEP:CONFig:AM0[?]",				scpi_cmd_rw_float,					AMU_REG_SWEEP_CONFIG_AM0			)	\
	SCPI_COMMAND("SWEEP:CONFig:AREA[?]",			scpi_cmd_rw_float,					AMU_REG_SWEEP_CONFIG_AREA			)	\
																																\
	SCPI_COMMAND("SUNSensor:TL?",					scpi_cmd_rw_float,					AMU_REG_SUNSENSOR_TL				)	\
	SCPI_COMMAND("SUNSensor:BL?",					scpi_cmd_rw_float,					AMU_REG_SUNSENSOR_BL				)	\
	SCPI_COMMAND("SUNSensor:BR?",					scpi_cmd_rw_float,					AMU_REG_SUNSENSOR_BR				)	\
	SCPI_COMMAND("SUNSensor:TR?",					scpi_cmd_rw_float,					AMU_REG_SUNSENSOR_TR				)	\
	SCPI_COMMAND("SUNSensor:YAW?",					scpi_cmd_rw_float,					AMU_REG_SUNSENSOR_YAW				)	\
	SCPI_COMMAND("SUNSensor:PITCH?",				scpi_cmd_rw_float,					AMU_REG_SUNSENSOR_PITCH				)	\
	SCPI_COMMAND("SUNSensor:FIT:YAW[?]",			scpi_cmd_rw_amu_coeff_t,			CMD_AUX_SUNSENSOR_FIT_YAW_COEFF		)	\
	SCPI_COMMAND("SUNSensor:FIT:PITCH[?]",			scpi_cmd_rw_amu_coeff_t,			CMD_AUX_SUNSENSOR_FIT_PITCH_COEFF	)	\
	SCPI_COMMAND("SUNSensor:FIT:SAVE",				scpi_cmd_execute,					CMD_EXEC_SUNSENSOR_COEFF_SAVE		)	\
	SCPI_COMMAND("SUNSensor:HVAL[?]",				scpi_cmd_rw_float,					CMD_AUX_SUNSENSOR_HVAL				)	\
	SCPI_COMMAND("SUNSensor:RVAL[?]",				scpi_cmd_rw_float,					CMD_AUX_SUNSENSOR_RVAL				)	\
	SCPI_COMMAND("SUNSensor:THRESHold[?]",			scpi_cmd_rw_float,					CMD_AUX_SUNSENSOR_THRESHOLD			)	\
																																\
	SCPI_COMMAND("MEASure:ADC:CH#[:RAW]?",			_scpi_cmd_measure_channel,			CMD_EXEC_MEAS_CHANNEL				)	\
	SCPI_COMMAND("MEASure:ADC:VOLTage[:RAW]?",		_scpi_cmd_measure_channel,			CMD_MEAS_CH_VOLTAGE					)	\
	SCPI_COMMAND("MEASure:ADC:CURRent[:RAW]?",		_scpi_cmd_measure_channel,			CMD_MEAS_CH_CURRENT					)	\
	SCPI_COMMAND("MEASure:ADC:TSENSor[:RAW]?",		_scpi_cmd_measure_channel,			CMD_MEAS_CH_TSENSOR					)	\
	SCPI_COMMAND("MEASure:ADC:TSENSOR0[:RAW]?",		_scpi_cmd_measure_channel,			CMD_MEAS_CH_TSENSOR_0				)	\
	SCPI_COMMAND("MEASure:ADC:TSENSOR1[:RAW]?",		_scpi_cmd_measure_channel,			CMD_MEAS_CH_TSENSOR_1				)	\
	SCPI_COMMAND("MEASure:ADC:TSENSOR2[:RAW]?",		_scpi_cmd_measure_channel,			CMD_MEAS_CH_TSENSOR_2				)	\
	SCPI_COMMAND("MEASure:ADC:TSENSORS[:RAW]?",		_scpi_cmd_measure_tsensors,			CMD_EXEC_MEAS_TSENSORS				)	\
	SCPI_COMMAND("MEASure:ADC:SSTL[:RAW]?",			_scpi_cmd_measure_channel,			CMD_MEAS_CH_SS_TL					)	\
	SCPI_COMMAND("MEASure:ADC:SSBL[:RAW]?",			_scpi_cmd_measure_channel,			CMD_MEAS_CH_SS_BL					)	\
	SCPI_COMMAND("MEASure:ADC:SSBR[:RAW]?",			_scpi_cmd_measure_channel,			CMD_MEAS_CH_SS_BR					)	\
	SCPI_COMMAND("MEASure:ADC:SSTR[:RAW]?",			_scpi_cmd_measure_channel,			CMD_MEAS_CH_SS_TR					)	\
	SCPI_COMMAND("MEASure:ADC:BIAS[:RAW]?",			_scpi_cmd_measure_channel,			CMD_MEAS_CH_BIAS					)	\
	SCPI_COMMAND("MEASure:ADC:OFFset[:RAW]?",		_scpi_cmd_measure_channel,			CMD_MEAS_CH_OFFSET					)	\
	SCPI_COMMAND("MEASure:ADC:TEMP[:RAW]?",			_scpi_cmd_measure_channel,			CMD_MEAS_CH_TEMP					)	\
	SCPI_COMMAND("MEASure:ADC:AVDD[:RAW]?",			_scpi_cmd_measure_channel,			CMD_MEAS_CH_AVDD					)	\
	SCPI_COMMAND("MEASure:ADC:IOVDD[:RAW]?",		_scpi_cmd_measure_channel,			CMD_MEAS_CH_IOVDD					)	\
	SCPI_COMMAND("MEASure:ADC:ALDO[:RAW]?",			_scpi_cmd_measure_channel,			CMD_MEAS_CH_ALDO					)	\
	SCPI_COMMAND("MEASure:ADC:DLDO[:RAW]?",			_scpi_cmd_measure_channel,			CMD_MEAS_CH_DLDO					)	\

#ifdef	__cplusplus
}
#endif

#endif /* SCPI_H_ */
