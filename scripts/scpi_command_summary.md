# AMU SCPI Command Reference

This document provides a complete reference for all SCPI commands supported by the AMU.

## Adc Commands

### `ADC:CALibrate:ALL:INTernal`

Calibrate all internal ADC channels

- **Handler:** scpi_cmd_execute
- **Command ID:** CMD_EXEC_ADC_CAL_ALL_INTERNAL
- **Parameters:** Command parameters vary by function
- **Returns:** Command acknowledgment

---

### `ADC:CALibrate:TSENSor`

Calibrate temperature sensor at 25°C

- **Handler:** scpi_cmd_rw_float
- **Command ID:** CMD_EXEC_TSENSOR_CAL_25C
- **Parameters:** Command parameters vary by function
- **Returns:** Command acknowledgment

---

### `ADC:CALibrate[?]`

Perform or query ADC calibration

- **Handler:** scpi_cmd_exec_qry_float
- **Command ID:** CMD_EXEC_ADC_CAL
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `ADC:CH#:CALibrate:FULL`

Perform full-scale calibration for channel

- **Handler:** scpi_cmd_execute
- **Command ID:** CMD_ADC_CH_CAL_FULL_SCALE
- **Parameters:** Channel number (0-15)
- **Returns:** Command acknowledgment

---

### `ADC:CH#:CALibrate:INTernal`

Perform internal calibration for channel

- **Handler:** scpi_cmd_execute
- **Command ID:** CMD_ADC_CH_CAL_INTERNAL
- **Parameters:** Channel number (0-15)
- **Returns:** Command acknowledgment

---

### `ADC:CH#:CALibrate:RESet`

Reset channel calibration to defaults

- **Handler:** scpi_cmd_execute
- **Command ID:** CMD_ADC_CH_CAL_RESET
- **Parameters:** Channel number (0-15)
- **Returns:** Command acknowledgment

---

### `ADC:CH#:CALibrate:SAVe`

Save channel calibration to EEPROM

- **Handler:** scpi_cmd_execute
- **Command ID:** CMD_ADC_CH_CAL_SAVE
- **Parameters:** Channel number (0-15)
- **Returns:** Command acknowledgment

---

### `ADC:CH#:CALibrate:ZERO`

Perform zero-scale calibration for channel

- **Handler:** scpi_cmd_execute
- **Command ID:** CMD_ADC_CH_CAL_ZERO_SCALE
- **Parameters:** Channel number (0-15)
- **Returns:** Command acknowledgment

---

### `ADC:CH#:FILTer[?]`

Set or query ADC channel filter settings

- **Handler:** scpi_cmd_rw_uint32_t
- **Command ID:** CMD_ADC_CH_FILTER
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `ADC:CH#:GAIN[?]`

Set or query channel gain calibration coefficient

- **Handler:** scpi_cmd_rw_int32_t
- **Command ID:** CMD_ADC_CH_GAIN_COEFF
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `ADC:CH#:MAX?`

Query maximum input range for current PGA setting

- **Handler:** scpi_cmd_rw_float
- **Command ID:** CMD_ADC_CH_PGA_MAX
- **Parameters:** Query only - no parameters
- **Returns:** Floating-point value

---

### `ADC:CH#:OFFset[?]`

Set or query channel offset calibration coefficient

- **Handler:** scpi_cmd_rw_int32_t
- **Command ID:** CMD_ADC_CH_OFFSET_COEFF
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `ADC:CH#:PGA[?]`

Set or query programmable gain amplifier setting

- **Handler:** scpi_cmd_rw_uint8_t
- **Command ID:** CMD_ADC_CH_PGA
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `ADC:CH#:RATE[?]`

Set or query ADC channel sample rate

- **Handler:** scpi_cmd_rw_float
- **Command ID:** CMD_ADC_CH_RATE
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `ADC:CH#:SAVE`

Save channel configuration to EEPROM

- **Handler:** scpi_cmd_execute
- **Command ID:** CMD_ADC_CH_SAVE
- **Parameters:** Channel number (0-15)
- **Returns:** Command acknowledgment

---

### `ADC:CH#:SETup[?]`

Set or query ADC channel setup configuration

- **Handler:** scpi_cmd_rw_uint16_t
- **Command ID:** CMD_ADC_CH_SETUP
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `ADC:CH#[?]`

Set or query ADC channel register value

- **Handler:** scpi_cmd_rw_uint16_t
- **Command ID:** CMD_ADC_CH_REG
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `ADC:CH:ACTIVE[?]`

Set or query active ADC channel mask

- **Handler:** scpi_cmd_rw_uint16_t
- **Command ID:** AMU_REG_SYSTEM_ADC_ACTIVE_CHANNELS
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `ADC:CURRent:CALibrate:FULL`

Set adc:current:calibrate:full value

- **Handler:** scpi_cmd_execute
- **Command ID:** CMD_USB_ADC_CURRENT_CAL_FULL
- **Parameters:** Command parameters vary by function
- **Returns:** Command acknowledgment

---

### `ADC:CURRent:CALibrate:RESet`

Set adc:current:calibrate:reset value

- **Handler:** scpi_cmd_execute
- **Command ID:** CMD_USB_ADC_CURRENT_CAL_RESET
- **Parameters:** Command parameters vary by function
- **Returns:** Command acknowledgment

---

### `ADC:CURRent:CALibrate:SAVe`

Set adc:current:calibrate:save value

- **Handler:** scpi_cmd_execute
- **Command ID:** CMD_USB_ADC_CURRENT_CAL_SAVE
- **Parameters:** Command parameters vary by function
- **Returns:** Command acknowledgment

---

### `ADC:CURRent:CALibrate:ZERO`

Set adc:current:calibrate:zero value

- **Handler:** scpi_cmd_execute
- **Command ID:** CMD_USB_ADC_CURRENT_CAL_ZERO
- **Parameters:** Command parameters vary by function
- **Returns:** Command acknowledgment

---

### `ADC:CURRent:GAIN[?]`

Set adc:current:gain[?] value

- **Handler:** scpi_cmd_rw_int32_t
- **Command ID:** CMD_USB_ADC_CURRENT_GAIN
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `ADC:CURRent:MAX:PGA#?`

Query adc:current:max:pga# value

- **Handler:** scpi_cmd_rw_float
- **Command ID:** CMD_ADC_CH_PGA_IMAX
- **Parameters:** Query only - no parameters
- **Returns:** Floating-point value

---

### `ADC:CURRent:MAX?`

Query adc:current:max value

- **Handler:** scpi_cmd_rw_float
- **Command ID:** CMD_USB_ADC_CURRENT_MAX
- **Parameters:** Query only - no parameters
- **Returns:** Floating-point value

---

### `ADC:CURRent:OFFset[?]`

Set adc:current:offset[?] value

- **Handler:** scpi_cmd_rw_int32_t
- **Command ID:** CMD_USB_ADC_CURRENT_OFFSET
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `ADC:CURRent:PGA[?]`

Set adc:current:pga[?] value

- **Handler:** scpi_cmd_rw_uint8_t
- **Command ID:** CMD_USB_ADC_CURRENT_PGA
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `ADC:SAVE:ALL:INTernal`

Save all internal calibrations to EEPROM

- **Handler:** scpi_cmd_execute
- **Command ID:** CMD_EXEC_ADC_CAL_SAVE_ALL_INTERNAL
- **Parameters:** Command parameters vary by function
- **Returns:** Command acknowledgment

---

### `ADC:STATus?`

Query adc:status value

- **Handler:** scpi_cmd_rw_uint32_t
- **Command ID:** AMU_REG_SYSTEM_STATUS_HRADC
- **Parameters:** Query only - no parameters
- **Returns:** 32-bit unsigned integer

---

### `ADC:VOLTage:CALibrate:FULL`

Set adc:voltage:calibrate:full value

- **Handler:** scpi_cmd_execute
- **Command ID:** CMD_USB_ADC_VOLTAGE_CAL_FULL
- **Parameters:** Command parameters vary by function
- **Returns:** Command acknowledgment

---

### `ADC:VOLTage:CALibrate:RESet`

Set adc:voltage:calibrate:reset value

- **Handler:** scpi_cmd_execute
- **Command ID:** CMD_USB_ADC_VOLTAGE_CAL_RESET
- **Parameters:** Command parameters vary by function
- **Returns:** Command acknowledgment

---

### `ADC:VOLTage:CALibrate:SAVe`

Set adc:voltage:calibrate:save value

- **Handler:** scpi_cmd_execute
- **Command ID:** CMD_USB_ADC_VOLTAGE_CAL_SAVE
- **Parameters:** Command parameters vary by function
- **Returns:** Command acknowledgment

---

### `ADC:VOLTage:CALibrate:ZERO`

Set adc:voltage:calibrate:zero value

- **Handler:** scpi_cmd_execute
- **Command ID:** CMD_USB_ADC_VOLTAGE_CAL_ZERO
- **Parameters:** Command parameters vary by function
- **Returns:** Command acknowledgment

---

### `ADC:VOLTage:GAIN[?]`

Set adc:voltage:gain[?] value

- **Handler:** scpi_cmd_rw_int32_t
- **Command ID:** CMD_USB_ADC_VOLTAGE_GAIN
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `ADC:VOLTage:MAX:PGA#?`

Query adc:voltage:max:pga# value

- **Handler:** scpi_cmd_rw_float
- **Command ID:** CMD_ADC_CH_PGA_VMAX
- **Parameters:** Query only - no parameters
- **Returns:** Floating-point value

---

### `ADC:VOLTage:MAX?`

Query adc:voltage:max value

- **Handler:** scpi_cmd_rw_float
- **Command ID:** CMD_USB_ADC_VOLTAGE_MAX
- **Parameters:** Query only - no parameters
- **Returns:** Floating-point value

---

### `ADC:VOLTage:OFFset[?]`

Set adc:voltage:offset[?] value

- **Handler:** scpi_cmd_rw_int32_t
- **Command ID:** CMD_USB_ADC_VOLTAGE_OFFSET
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `ADC:VOLTage:PGA[?]`

Set adc:voltage:pga[?] value

- **Handler:** scpi_cmd_rw_uint8_t
- **Command ID:** CMD_USB_ADC_VOLTAGE_PGA
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

## Dac Commands

### `DAC:CALibrate:SAVe`

Set dac:calibrate:save value

- **Handler:** scpi_cmd_execute
- **Command ID:** CMD_EXEC_DAC_CAL_SAVE
- **Parameters:** Command parameters vary by function
- **Returns:** Command acknowledgment

---

### `DAC:CALibrate[?]`

Set dac:calibrate[?] value

- **Handler:** scpi_cmd_rw_float
- **Command ID:** CMD_EXEC_DAC_CAL
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `DAC:CURRent:RAW[?]`

Set dac:current:raw[?] value

- **Handler:** scpi_cmd_rw_uint16_t
- **Command ID:** CMD_AUX_DAC_CURRENT_RAW
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `DAC:CURRent[?]`

Set dac:current[?] value

- **Handler:** scpi_cmd_rw_float
- **Command ID:** CMD_AUX_DAC_CURRENT
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `DAC:GAIN:CORRection[?]`

Set dac:gain:correction[?] value

- **Handler:** scpi_cmd_rw_float
- **Command ID:** CMD_AUX_DAC_GAIN_CORRECTION
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `DAC:OFFset:CORRection[?]`

Set dac:offset:correction[?] value

- **Handler:** scpi_cmd_rw_float
- **Command ID:** CMD_AUX_DAC_OFFSET_CORRECTION
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `DAC:OFFset[?]`

Set dac:offset[?] value

- **Handler:** scpi_cmd_rw_uint16_t
- **Command ID:** CMD_AUX_DAC_OFFSET
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `DAC:STATE[?]`

Set dac:state[?] value

- **Handler:** scpi_cmd_rw_uint8_t
- **Command ID:** CMD_AUX_DAC_STATE
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `DAC:VOLTage:RAW[?]`

Set dac:voltage:raw[?] value

- **Handler:** scpi_cmd_rw_uint16_t
- **Command ID:** CMD_AUX_DAC_VOLTAGE_RAW
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `DAC:VOLTage[?]`

Set dac:voltage[?] value

- **Handler:** scpi_cmd_rw_float
- **Command ID:** CMD_AUX_DAC_VOLTAGE
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

## Dut Commands

### `DUT:COVERglass[?]`

Set dut:coverglass[?] value

- **Handler:** scpi_cmd_rw_uint8_t
- **Command ID:** CMD_DUT_COVERGLASS
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `DUT:DOSE[?]`

Set dut:dose[?] value

- **Handler:** scpi_cmd_rw_float
- **Command ID:** CMD_DUT_DOSE
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `DUT:ENERGY[?]`

Set dut:energy[?] value

- **Handler:** scpi_cmd_rw_float
- **Command ID:** CMD_DUT_ENERGY
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `DUT:INTERconnect[?]`

Set dut:interconnect[?] value

- **Handler:** scpi_cmd_rw_uint8_t
- **Command ID:** CMD_DUT_INTERCONNECT
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `DUT:JUNCtion[?]`

Set dut:junction[?] value

- **Handler:** scpi_cmd_rw_uint8_t
- **Command ID:** CMD_DUT_JUNCTION
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `DUT:MANufacturer[?]`

Set dut:manufacturer[?] value

- **Handler:** _scpi_cmd_query_str
- **Command ID:** CMD_DUT_MANUFACTURER
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `DUT:MODel[?]`

Set dut:model[?] value

- **Handler:** _scpi_cmd_query_str
- **Command ID:** CMD_DUT_MODEL
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `DUT:NOTEs[?]`

Set dut:notes[?] value

- **Handler:** _scpi_cmd_query_str
- **Command ID:** CMD_DUT_NOTES
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `DUT:SERialnumber[?]`

Set dut:serialnumber[?] value

- **Handler:** _scpi_cmd_query_str
- **Command ID:** CMD_DUT_SERIAL_NUMBER
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `DUT:TECHnology[?]`

Set dut:technology[?] value

- **Handler:** _scpi_cmd_query_str
- **Command ID:** CMD_DUT_TECHNOLOGY
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `DUT:TSENSor:FIT[?]`

Set dut:tsensor:fit[?] value

- **Handler:** scpi_cmd_rw_amu_coeff_t
- **Command ID:** CMD_DUT_TSENSOR_FIT
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `DUT:TSENSor:NUMber[?]`

Set dut:tsensor:number[?] value

- **Handler:** scpi_cmd_rw_uint8_t
- **Command ID:** CMD_DUT_TSENSOR_NUMBER
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `DUT:TSENSor:TYPE[?]`

Set dut:tsensor:type[?] value

- **Handler:** scpi_cmd_rw_uint8_t
- **Command ID:** CMD_DUT_TSENSOR_TYPE
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

## Heater Commands

### `HEATer:PID:SAVE`

Set heater:pid:save value

- **Handler:** scpi_cmd_execute
- **Command ID:** CMD_EXEC_HEATER_PID_SAVE
- **Parameters:** Command parameters vary by function
- **Returns:** Command acknowledgment

---

### `HEATer:PID[?]`

Set heater:pid[?] value

- **Handler:** scpi_cmd_rw_amu_pid_t
- **Command ID:** CMD_AUX_HEATER_PID
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `HEATer:SETpoint[?]`

Set heater:setpoint[?] value

- **Handler:** scpi_cmd_rw_float
- **Command ID:** CMD_AUX_HEATER_SETPOINT
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `HEATer:STATE[?]`

Set heater:state[?] value

- **Handler:** scpi_cmd_rw_uint8_t
- **Command ID:** CMD_AUX_HEATER_STATE
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

## Ieee488 Commands

### `*CLS`

Clear status command - clears device status registers

- **Handler:** SCPI_CoreCls
- **Command ID:** 0
- **Parameters:** Command parameters vary by function
- **Returns:** Command acknowledgment

---

### `*ESE?`

Query event status enable register

- **Handler:** SCPI_CoreEseQ
- **Command ID:** 0
- **Parameters:** Query only - no parameters
- **Returns:** Numeric value

---

### `*ESR?`

Query event status register

- **Handler:** SCPI_CoreEsrQ
- **Command ID:** 0
- **Parameters:** Query only - no parameters
- **Returns:** Numeric value

---

### `*IDN?`

Query device identification string

- **Handler:** SCPI_CoreIdnQ
- **Command ID:** 0
- **Parameters:** Query only - no parameters
- **Returns:** Numeric value

---

### `*OPC`

Operation complete command

- **Handler:** SCPI_CoreOpc
- **Command ID:** 0
- **Parameters:** Command parameters vary by function
- **Returns:** Command acknowledgment

---

### `*OPC?`

Query operation complete status

- **Handler:** SCPI_CoreOpcQ
- **Command ID:** 0
- **Parameters:** Query only - no parameters
- **Returns:** Numeric value

---

### `*RST`

Reset device to default state

- **Handler:** SCPI_CoreRst
- **Command ID:** 0
- **Parameters:** Command parameters vary by function
- **Returns:** Command acknowledgment

---

### `*SRE`

Set service request enable register

- **Handler:** SCPI_CoreSre
- **Command ID:** 0
- **Parameters:** Command parameters vary by function
- **Returns:** Command acknowledgment

---

### `*SRE?`

Query service request enable register

- **Handler:** SCPI_CoreSreQ
- **Command ID:** 0
- **Parameters:** Query only - no parameters
- **Returns:** Numeric value

---

### `*STB?`

Query status byte register

- **Handler:** SCPI_CoreStbQ
- **Command ID:** 0
- **Parameters:** Query only - no parameters
- **Returns:** Numeric value

---

### `*TST?`

Self-test query

- **Handler:** scpi_cmd_execute
- **Command ID:** CMD_SYSTEM_NO_CMD
- **Parameters:** Query only - no parameters
- **Returns:** Numeric value

---

### `*WAI`

Wait for all operations to complete

- **Handler:** SCPI_CoreWai
- **Command ID:** 0
- **Parameters:** Command parameters vary by function
- **Returns:** Command acknowledgment

---

## Measurement Commands

### `MEASure:ADC:ACTive[:RAW]?`

Measure all active ADC channels

- **Handler:** _scpi_cmd_measure_active_ch
- **Command ID:** CMD_EXEC_MEAS_ACTIVE_CHANNELS
- **Parameters:** Query only - no parameters
- **Returns:** Numeric value

---

### `MEASure:ADC:ALDO[:RAW]?`

Query measure:adc:aldo[:raw] value

- **Handler:** _scpi_cmd_measure_channel
- **Command ID:** CMD_MEAS_CH_ALDO
- **Parameters:** Query only - no parameters
- **Returns:** Numeric value

---

### `MEASure:ADC:AVDD[:RAW]?`

Query measure:adc:avdd[:raw] value

- **Handler:** _scpi_cmd_measure_channel
- **Command ID:** CMD_MEAS_CH_AVDD
- **Parameters:** Query only - no parameters
- **Returns:** Numeric value

---

### `MEASure:ADC:BIAS[:RAW]?`

Query measure:adc:bias[:raw] value

- **Handler:** _scpi_cmd_measure_channel
- **Command ID:** CMD_MEAS_CH_BIAS
- **Parameters:** Query only - no parameters
- **Returns:** Numeric value

---

### `MEASure:ADC:CH#[:RAW]?`

Query measure:adc:ch#[:raw] value

- **Handler:** _scpi_cmd_measure_channel
- **Command ID:** CMD_EXEC_MEAS_CHANNEL
- **Parameters:** Query only - no parameters
- **Returns:** Numeric value

---

### `MEASure:ADC:CURRent[:RAW]?`

Query measure:adc:current[:raw] value

- **Handler:** _scpi_cmd_measure_channel
- **Command ID:** CMD_MEAS_CH_CURRENT
- **Parameters:** Query only - no parameters
- **Returns:** Numeric value

---

### `MEASure:ADC:DLDO[:RAW]?`

Query measure:adc:dldo[:raw] value

- **Handler:** _scpi_cmd_measure_channel
- **Command ID:** CMD_MEAS_CH_DLDO
- **Parameters:** Query only - no parameters
- **Returns:** Numeric value

---

### `MEASure:ADC:IOVDD[:RAW]?`

Query measure:adc:iovdd[:raw] value

- **Handler:** _scpi_cmd_measure_channel
- **Command ID:** CMD_MEAS_CH_IOVDD
- **Parameters:** Query only - no parameters
- **Returns:** Numeric value

---

### `MEASure:ADC:OFFset[:RAW]?`

Query measure:adc:offset[:raw] value

- **Handler:** _scpi_cmd_measure_channel
- **Command ID:** CMD_MEAS_CH_OFFSET
- **Parameters:** Query only - no parameters
- **Returns:** Numeric value

---

### `MEASure:ADC:SSBL[:RAW]?`

Query measure:adc:ssbl[:raw] value

- **Handler:** _scpi_cmd_measure_channel
- **Command ID:** CMD_MEAS_CH_SS_BL
- **Parameters:** Query only - no parameters
- **Returns:** Numeric value

---

### `MEASure:ADC:SSBR[:RAW]?`

Query measure:adc:ssbr[:raw] value

- **Handler:** _scpi_cmd_measure_channel
- **Command ID:** CMD_MEAS_CH_SS_BR
- **Parameters:** Query only - no parameters
- **Returns:** Numeric value

---

### `MEASure:ADC:SSTL[:RAW]?`

Query measure:adc:sstl[:raw] value

- **Handler:** _scpi_cmd_measure_channel
- **Command ID:** CMD_MEAS_CH_SS_TL
- **Parameters:** Query only - no parameters
- **Returns:** Numeric value

---

### `MEASure:ADC:SSTR[:RAW]?`

Query measure:adc:sstr[:raw] value

- **Handler:** _scpi_cmd_measure_channel
- **Command ID:** CMD_MEAS_CH_SS_TR
- **Parameters:** Query only - no parameters
- **Returns:** Numeric value

---

### `MEASure:ADC:TEMP[:RAW]?`

Query measure:adc:temp[:raw] value

- **Handler:** _scpi_cmd_measure_channel
- **Command ID:** CMD_MEAS_CH_TEMP
- **Parameters:** Query only - no parameters
- **Returns:** Numeric value

---

### `MEASure:ADC:TSENSOR0[:RAW]?`

Query measure:adc:tsensor0[:raw] value

- **Handler:** _scpi_cmd_measure_channel
- **Command ID:** CMD_MEAS_CH_TSENSOR_0
- **Parameters:** Query only - no parameters
- **Returns:** Numeric value

---

### `MEASure:ADC:TSENSOR1[:RAW]?`

Query measure:adc:tsensor1[:raw] value

- **Handler:** _scpi_cmd_measure_channel
- **Command ID:** CMD_MEAS_CH_TSENSOR_1
- **Parameters:** Query only - no parameters
- **Returns:** Numeric value

---

### `MEASure:ADC:TSENSOR2[:RAW]?`

Query measure:adc:tsensor2[:raw] value

- **Handler:** _scpi_cmd_measure_channel
- **Command ID:** CMD_MEAS_CH_TSENSOR_2
- **Parameters:** Query only - no parameters
- **Returns:** Numeric value

---

### `MEASure:ADC:TSENSORS[:RAW]?`

Query measure:adc:tsensors[:raw] value

- **Handler:** _scpi_cmd_measure_tsensors
- **Command ID:** CMD_EXEC_MEAS_TSENSORS
- **Parameters:** Query only - no parameters
- **Returns:** Numeric value

---

### `MEASure:ADC:TSENSor[:RAW]?`

Query measure:adc:tsensor[:raw] value

- **Handler:** _scpi_cmd_measure_channel
- **Command ID:** CMD_MEAS_CH_TSENSOR
- **Parameters:** Query only - no parameters
- **Returns:** Numeric value

---

### `MEASure:ADC:VOLTage[:RAW]?`

Query measure:adc:voltage[:raw] value

- **Handler:** _scpi_cmd_measure_channel
- **Command ID:** CMD_MEAS_CH_VOLTAGE
- **Parameters:** Query only - no parameters
- **Returns:** Numeric value

---

### `MEASure:INTERNALvoltages?`

Measure internal supply voltages

- **Handler:** scpi_cmd_rw_amu_int_volt_t
- **Command ID:** CMD_EXEC_MEAS_INTERNAL_VOLTAGES
- **Parameters:** Query only - no parameters
- **Returns:** Numeric value

---

### `MEASure:PRESSure?`

Measure pressure sensor data

- **Handler:** scpi_cmd_rw_press_data_t
- **Command ID:** CMD_EXEC_MEAS_PRESSURE_SENSOR
- **Parameters:** Query only - no parameters
- **Returns:** Numeric value

---

### `MEASure:SUNSensor?`

Measure sun sensor angles

- **Handler:** scpi_cmd_rw_ss_angle_t
- **Command ID:** CMD_EXEC_MEAS_SUN_SENSOR
- **Parameters:** Query only - no parameters
- **Returns:** Numeric value

---

## Memory Commands

### `MEMory:ADC:CH#:GAIN[?]`

Set memory:adc:ch#:gain[?] value

- **Handler:** scpi_cmd_rw_uint32_t
- **Command ID:** CMD_USB_EEPROM_GAIN
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `MEMory:ADC:CH#:OFFset[?]`

Set memory:adc:ch#:offset[?] value

- **Handler:** scpi_cmd_rw_uint32_t
- **Command ID:** CMD_USB_EEPROM_OFFSET
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `MEMory:CURRent:GAIN#[?]`

Set memory:current:gain#[?] value

- **Handler:** scpi_cmd_rw_uint32_t
- **Command ID:** CMD_USB_EEPROM_CURRENT_GAIN
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `MEMory:CURRent:OFFset#[?]`

Set memory:current:offset#[?] value

- **Handler:** scpi_cmd_rw_uint32_t
- **Command ID:** CMD_USB_EEPROM_CURRENT_OFFSET
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `MEMory:ERASE:ALL`

Set memory:erase:all value

- **Handler:** scpi_cmd_execute
- **Command ID:** CMD_USB_EEPROM_ERASE_ALL
- **Parameters:** Command parameters vary by function
- **Returns:** Command acknowledgment

---

### `MEMory:ERASE:CONFig`

Set memory:erase:config value

- **Handler:** scpi_cmd_execute
- **Command ID:** CMD_USB_EEPROM_ERASE_CONFIG
- **Parameters:** Command parameters vary by function
- **Returns:** Command acknowledgment

---

### `MEMory:VOLTage:GAIN#[?]`

Set memory:voltage:gain#[?] value

- **Handler:** scpi_cmd_rw_uint32_t
- **Command ID:** CMD_USB_EEPROM_VOLTAGE_GAIN
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `MEMory:VOLTage:OFFset#[?]`

Set memory:voltage:offset#[?] value

- **Handler:** scpi_cmd_rw_uint32_t
- **Command ID:** CMD_USB_EEPROM_VOLTAGE_OFFSET
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

## Status Commands

### `STATus:PRESet`

Set status:preset value

- **Handler:** SCPI_StatusPreset
- **Command ID:** 0
- **Parameters:** Command parameters vary by function
- **Returns:** Command acknowledgment

---

### `STATus:QUEStionable:ENABle`

Set status:questionable:enable value

- **Handler:** SCPI_StatusQuestionableEnable
- **Command ID:** 0
- **Parameters:** Command parameters vary by function
- **Returns:** Command acknowledgment

---

### `STATus:QUEStionable:ENABle?`

Query status:questionable:enable value

- **Handler:** SCPI_StatusQuestionableEnableQ
- **Command ID:** 0
- **Parameters:** Query only - no parameters
- **Returns:** Numeric value

---

## Sunsensor Commands

### `SUNSensor:BL?`

Query sunsensor:bl value

- **Handler:** scpi_cmd_rw_float
- **Command ID:** AMU_REG_SUNSENSOR_BL
- **Parameters:** Query only - no parameters
- **Returns:** Floating-point value

---

### `SUNSensor:BR?`

Query sunsensor:br value

- **Handler:** scpi_cmd_rw_float
- **Command ID:** AMU_REG_SUNSENSOR_BR
- **Parameters:** Query only - no parameters
- **Returns:** Floating-point value

---

### `SUNSensor:FIT:PITCH[?]`

Set sunsensor:fit:pitch[?] value

- **Handler:** scpi_cmd_rw_amu_coeff_t
- **Command ID:** CMD_AUX_SUNSENSOR_FIT_PITCH_COEFF
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `SUNSensor:FIT:SAVE`

Set sunsensor:fit:save value

- **Handler:** scpi_cmd_execute
- **Command ID:** CMD_EXEC_SUNSENSOR_COEFF_SAVE
- **Parameters:** Command parameters vary by function
- **Returns:** Command acknowledgment

---

### `SUNSensor:FIT:YAW[?]`

Set sunsensor:fit:yaw[?] value

- **Handler:** scpi_cmd_rw_amu_coeff_t
- **Command ID:** CMD_AUX_SUNSENSOR_FIT_YAW_COEFF
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `SUNSensor:HVAL[?]`

Set sunsensor:hval[?] value

- **Handler:** scpi_cmd_rw_float
- **Command ID:** CMD_AUX_SUNSENSOR_HVAL
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `SUNSensor:PITCH?`

Query sunsensor:pitch value

- **Handler:** scpi_cmd_rw_float
- **Command ID:** AMU_REG_SUNSENSOR_PITCH
- **Parameters:** Query only - no parameters
- **Returns:** Floating-point value

---

### `SUNSensor:RVAL[?]`

Set sunsensor:rval[?] value

- **Handler:** scpi_cmd_rw_float
- **Command ID:** CMD_AUX_SUNSENSOR_RVAL
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `SUNSensor:THRESHold[?]`

Set sunsensor:threshold[?] value

- **Handler:** scpi_cmd_rw_float
- **Command ID:** CMD_AUX_SUNSENSOR_THRESHOLD
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `SUNSensor:TL?`

Query sunsensor:tl value

- **Handler:** scpi_cmd_rw_float
- **Command ID:** AMU_REG_SUNSENSOR_TL
- **Parameters:** Query only - no parameters
- **Returns:** Floating-point value

---

### `SUNSensor:TR?`

Query sunsensor:tr value

- **Handler:** scpi_cmd_rw_float
- **Command ID:** AMU_REG_SUNSENSOR_TR
- **Parameters:** Query only - no parameters
- **Returns:** Floating-point value

---

### `SUNSensor:YAW?`

Query sunsensor:yaw value

- **Handler:** scpi_cmd_rw_float
- **Command ID:** AMU_REG_SUNSENSOR_YAW
- **Parameters:** Query only - no parameters
- **Returns:** Floating-point value

---

### `SUNSensor?`

Query sunsensor value

- **Handler:** _scpi_read_ptr
- **Command ID:** AMU_REG_DATA_PTR_SUNSENSOR
- **Parameters:** Query only - no parameters
- **Returns:** Numeric value

---

## Sweep Commands

### `SWEEP:AES:DECode`

Set sweep:aes:decode value

- **Handler:** scpi_cmd_execute
- **Command ID:** CMD_SWEEP_AES_DECODE
- **Parameters:** Command parameters vary by function
- **Returns:** Command acknowledgment

---

### `SWEEP:AES:ENCode`

Set sweep:aes:encode value

- **Handler:** scpi_cmd_execute
- **Command ID:** CMD_SWEEP_AES_ENCODE
- **Parameters:** Command parameters vary by function
- **Returns:** Command acknowledgment

---

### `SWEEP:CONFig`

Set sweep:config value

- **Handler:** _scpi_write_config_ptr
- **Command ID:** AMU_REG_DATA_PTR_SWEEP_CONFIG
- **Parameters:** Command parameters vary by function
- **Returns:** Command acknowledgment

---

### `SWEEP:CONFig:ADC:AVERages[?]`

Set sweep:config:adc:averages[?] value

- **Handler:** scpi_cmd_rw_uint8_t
- **Command ID:** AMU_REG_SWEEP_CONFIG_ADC_AVERAGES
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `SWEEP:CONFig:AM0[?]`

Set sweep:config:am0[?] value

- **Handler:** scpi_cmd_rw_float
- **Command ID:** AMU_REG_SWEEP_CONFIG_AM0
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `SWEEP:CONFig:AREA[?]`

Set sweep:config:area[?] value

- **Handler:** scpi_cmd_rw_float
- **Command ID:** AMU_REG_SWEEP_CONFIG_AREA
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `SWEEP:CONFig:AVERages[?]`

Set sweep:config:averages[?] value

- **Handler:** scpi_cmd_rw_uint8_t
- **Command ID:** AMU_REG_SWEEP_CONFIG_AVERAGES
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `SWEEP:CONFig:DELAY[?]`

Set sweep:config:delay[?] value

- **Handler:** scpi_cmd_rw_uint8_t
- **Command ID:** AMU_REG_SWEEP_CONFIG_DELAY
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `SWEEP:CONFig:GAIN[?]`

Set sweep:config:gain[?] value

- **Handler:** scpi_cmd_rw_uint8_t
- **Command ID:** AMU_REG_SWEEP_CONFIG_DAC_GAIN
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `SWEEP:CONFig:NUMpoints[?]`

Set sweep:config:numpoints[?] value

- **Handler:** scpi_cmd_rw_uint8_t
- **Command ID:** AMU_REG_SWEEP_CONFIG_NUM_POINTS
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `SWEEP:CONFig:POWER[?]`

Set sweep:config:power[?] value

- **Handler:** scpi_cmd_rw_uint8_t
- **Command ID:** AMU_REG_SWEEP_CONFIG_PWR_MODE
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `SWEEP:CONFig:RATIO[?]`

Set sweep:config:ratio[?] value

- **Handler:** scpi_cmd_rw_uint8_t
- **Command ID:** AMU_REG_SWEEP_CONFIG_RATIO
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `SWEEP:CONFig:SAVe`

Set sweep:config:save value

- **Handler:** scpi_cmd_execute
- **Command ID:** CMD_SWEEP_CONFIG_SAVE
- **Parameters:** Command parameters vary by function
- **Returns:** Command acknowledgment

---

### `SWEEP:CONFig:TYPE[?]`

Set sweep:config:type[?] value

- **Handler:** scpi_cmd_rw_uint8_t
- **Command ID:** AMU_REG_SWEEP_CONFIG_TYPE
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `SWEEP:CONFig?`

Query sweep:config value

- **Handler:** _scpi_read_ptr
- **Command ID:** AMU_REG_DATA_PTR_SWEEP_CONFIG
- **Parameters:** Query only - no parameters
- **Returns:** Numeric value

---

### `SWEEP:CURRent`

Set sweep:current value

- **Handler:** _scpi_write_sweep_ptr
- **Command ID:** AMU_REG_DATA_PTR_CURRENT
- **Parameters:** Command parameters vary by function
- **Returns:** Command acknowledgment

---

### `SWEEP:CURRent?`

Query sweep:current value

- **Handler:** _scpi_read_ptr
- **Command ID:** AMU_REG_DATA_PTR_CURRENT
- **Parameters:** Query only - no parameters
- **Returns:** Numeric value

---

### `SWEEP:DATApoint:SAVE`

Set sweep:datapoint:save value

- **Handler:** scpi_cmd_execute
- **Command ID:** CMD_SWEEP_DATAPOINT_SAVE
- **Parameters:** Command parameters vary by function
- **Returns:** Command acknowledgment

---

### `SWEEP:DISable`

Set sweep:disable value

- **Handler:** scpi_cmd_execute
- **Command ID:** CMD_SWEEP_DISABLE
- **Parameters:** Command parameters vary by function
- **Returns:** Command acknowledgment

---

### `SWEEP:EEPROM:LOAD`

Set sweep:eeprom:load value

- **Handler:** scpi_cmd_execute
- **Command ID:** CMD_SWEEP_IV_LOAD_FROM_EEPROM
- **Parameters:** Command parameters vary by function
- **Returns:** Command acknowledgment

---

### `SWEEP:EEPROM:SAVE`

Set sweep:eeprom:save value

- **Handler:** scpi_cmd_execute
- **Command ID:** CMD_SWEEP_IV_SAVE_TO_EEPROM
- **Parameters:** Command parameters vary by function
- **Returns:** Command acknowledgment

---

### `SWEEP:ENAble`

Set sweep:enable value

- **Handler:** scpi_cmd_execute
- **Command ID:** CMD_SWEEP_ENABLE
- **Parameters:** Command parameters vary by function
- **Returns:** Command acknowledgment

---

### `SWEEP:META`

Set sweep:meta value

- **Handler:** _scpi_write_meta_ptr
- **Command ID:** AMU_REG_DATA_PTR_SWEEP_META
- **Parameters:** Command parameters vary by function
- **Returns:** Command acknowledgment

---

### `SWEEP:META:ADC?`

Query sweep:meta:adc value

- **Handler:** scpi_cmd_rw_float
- **Command ID:** AMU_REG_SWEEP_META_ADC
- **Parameters:** Query only - no parameters
- **Returns:** Floating-point value

---

### `SWEEP:META:CRC?`

Query sweep:meta:crc value

- **Handler:** scpi_cmd_rw_uint32_t
- **Command ID:** AMU_REG_SWEEP_META_CRC
- **Parameters:** Query only - no parameters
- **Returns:** 32-bit unsigned integer

---

### `SWEEP:META:EFF?`

Query sweep:meta:eff value

- **Handler:** scpi_cmd_rw_float
- **Command ID:** AMU_REG_SWEEP_META_EFF
- **Parameters:** Query only - no parameters
- **Returns:** Floating-point value

---

### `SWEEP:META:FF?`

Query sweep:meta:ff value

- **Handler:** scpi_cmd_rw_float
- **Command ID:** AMU_REG_SWEEP_META_FF
- **Parameters:** Query only - no parameters
- **Returns:** Floating-point value

---

### `SWEEP:META:IMAX?`

Query sweep:meta:imax value

- **Handler:** scpi_cmd_rw_float
- **Command ID:** AMU_REG_SWEEP_META_IMAX
- **Parameters:** Query only - no parameters
- **Returns:** Floating-point value

---

### `SWEEP:META:ISC?`

Query sweep:meta:isc value

- **Handler:** scpi_cmd_rw_float
- **Command ID:** AMU_REG_SWEEP_META_ISC
- **Parameters:** Query only - no parameters
- **Returns:** Floating-point value

---

### `SWEEP:META:PMAX?`

Query sweep:meta:pmax value

- **Handler:** scpi_cmd_rw_float
- **Command ID:** AMU_REG_SWEEP_META_PMAX
- **Parameters:** Query only - no parameters
- **Returns:** Floating-point value

---

### `SWEEP:META:TIMEstamp?`

Query sweep:meta:timestamp value

- **Handler:** scpi_cmd_rw_uint32_t
- **Command ID:** AMU_REG_SWEEP_META_TIMESTAMP
- **Parameters:** Query only - no parameters
- **Returns:** 32-bit unsigned integer

---

### `SWEEP:META:TSENSor:END?`

Query sweep:meta:tsensor:end value

- **Handler:** scpi_cmd_rw_float
- **Command ID:** AMU_REG_SWEEP_META_TSENSOR_END
- **Parameters:** Query only - no parameters
- **Returns:** Floating-point value

---

### `SWEEP:META:TSENSor:START?`

Query sweep:meta:tsensor:start value

- **Handler:** scpi_cmd_rw_float
- **Command ID:** AMU_REG_SWEEP_META_TSENSOR_START
- **Parameters:** Query only - no parameters
- **Returns:** Floating-point value

---

### `SWEEP:META:VMAX?`

Query sweep:meta:vmax value

- **Handler:** scpi_cmd_rw_float
- **Command ID:** AMU_REG_SWEEP_META_VMAX
- **Parameters:** Query only - no parameters
- **Returns:** Floating-point value

---

### `SWEEP:META:VOC?`

Query sweep:meta:voc value

- **Handler:** scpi_cmd_rw_float
- **Command ID:** AMU_REG_SWEEP_META_VOC
- **Parameters:** Query only - no parameters
- **Returns:** Floating-point value

---

### `SWEEP:META?`

Query sweep:meta value

- **Handler:** _scpi_read_ptr
- **Command ID:** AMU_REG_DATA_PTR_SWEEP_META
- **Parameters:** Query only - no parameters
- **Returns:** Numeric value

---

### `SWEEP:PITCH`

Set sweep:pitch value

- **Handler:** _scpi_write_sweep_ptr
- **Command ID:** AMU_REG_DATA_PTR_SS_PITCH
- **Parameters:** Command parameters vary by function
- **Returns:** Command acknowledgment

---

### `SWEEP:PITCH?`

Query sweep:pitch value

- **Handler:** _scpi_read_ptr
- **Command ID:** AMU_REG_DATA_PTR_SS_PITCH
- **Parameters:** Query only - no parameters
- **Returns:** Numeric value

---

### `SWEEP:TIMEstamp?`

Query sweep:timestamp value

- **Handler:** _scpi_read_ptr
- **Command ID:** AMU_REG_DATA_PTR_TIMESTAMP
- **Parameters:** Query only - no parameters
- **Returns:** Numeric value

---

### `SWEEP:TRIGger`

Set sweep:trigger value

- **Handler:** scpi_cmd_execute
- **Command ID:** CMD_SWEEP_TRIG_SWEEP
- **Parameters:** Command parameters vary by function
- **Returns:** Command acknowledgment

---

### `SWEEP:TRIGger:INITialize`

Set sweep:trigger:initialize value

- **Handler:** scpi_cmd_execute
- **Command ID:** CMD_SWEEP_TRIG_INIT
- **Parameters:** Command parameters vary by function
- **Returns:** Command acknowledgment

---

### `SWEEP:TRIGger:ISC?`

Query sweep:trigger:isc value

- **Handler:** scpi_cmd_rw_amu_meas_t
- **Command ID:** CMD_SWEEP_TRIG_ISC
- **Parameters:** Query only - no parameters
- **Returns:** Numeric value

---

### `SWEEP:TRIGger:USB`

Set sweep:trigger:usb value

- **Handler:** scpi_cmd_execute
- **Command ID:** CMD_USB_SWEEP_TRIGGER
- **Parameters:** Command parameters vary by function
- **Returns:** Command acknowledgment

---

### `SWEEP:TRIGger:VOC?`

Query sweep:trigger:voc value

- **Handler:** scpi_cmd_rw_amu_meas_t
- **Command ID:** CMD_SWEEP_TRIG_VOC
- **Parameters:** Query only - no parameters
- **Returns:** Numeric value

---

### `SWEEP:VOLTage`

Set sweep:voltage value

- **Handler:** _scpi_write_sweep_ptr
- **Command ID:** AMU_REG_DATA_PTR_VOLTAGE
- **Parameters:** Command parameters vary by function
- **Returns:** Command acknowledgment

---

### `SWEEP:VOLTage?`

Query sweep:voltage value

- **Handler:** _scpi_read_ptr
- **Command ID:** AMU_REG_DATA_PTR_VOLTAGE
- **Parameters:** Query only - no parameters
- **Returns:** Numeric value

---

### `SWEEP:YAW`

Set sweep:yaw value

- **Handler:** _scpi_write_sweep_ptr
- **Command ID:** AMU_REG_DATA_PTR_SS_YAW
- **Parameters:** Command parameters vary by function
- **Returns:** Command acknowledgment

---

### `SWEEP:YAW?`

Query sweep:yaw value

- **Handler:** _scpi_read_ptr
- **Command ID:** AMU_REG_DATA_PTR_SS_YAW
- **Parameters:** Query only - no parameters
- **Returns:** Numeric value

---

## System Commands

### `SYSTem:BOOTloader`

Enter bootloader mode for firmware update

- **Handler:** scpi_cmd_execute
- **Command ID:** CMD_USB_SYSTEM_ENTER_BOOTLOADER
- **Parameters:** Command parameters vary by function
- **Returns:** Command acknowledgment

---

### `SYSTem:CONFig:CURRent:GAIN[?]`

Set system:config:current:gain[?] value

- **Handler:** scpi_cmd_rw_float
- **Command ID:** CMD_USB_SYSTEM_CONFIG_CURR_GAIN
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `SYSTem:CONFig:CURRent:Rsense[?]`

Set system:config:current:rsense[?] value

- **Handler:** scpi_cmd_rw_float
- **Command ID:** CMD_USB_SYSTEM_CONFIG_CURR_RSENSE
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `SYSTem:CONFig:SAVE`

Set system:config:save value

- **Handler:** scpi_cmd_execute
- **Command ID:** CMD_USB_SYSTEM_CONFIG_SAVE
- **Parameters:** Command parameters vary by function
- **Returns:** Command acknowledgment

---

### `SYSTem:CONFig:VOLTage:R1[?]`

Set system:config:voltage:r1[?] value

- **Handler:** scpi_cmd_rw_float
- **Command ID:** CMD_USB_SYSTEM_CONFIG_VOLT_R1
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `SYSTem:CONFig:VOLTage:R2[?]`

Set system:config:voltage:r2[?] value

- **Handler:** scpi_cmd_rw_float
- **Command ID:** CMD_USB_SYSTEM_CONFIG_VOLT_R2
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `SYSTem:DEBug#?`

Query system:debug# value

- **Handler:** scpi_cmd_rw_float
- **Command ID:** CMD_USB_SYSTEM_DEBUG
- **Parameters:** Query only - no parameters
- **Returns:** Floating-point value

---

### `SYSTem:ERRor:COUNt?`

Query number of errors in error queue

- **Handler:** SCPI_SystemErrorCountQ
- **Command ID:** 0
- **Parameters:** Query only - no parameters
- **Returns:** Numeric value

---

### `SYSTem:ERRor[:NEXT]?`

Query next error in error queue

- **Handler:** SCPI_SystemErrorNextQ
- **Command ID:** 0
- **Parameters:** Query only - no parameters
- **Returns:** Numeric value

---

### `SYSTem:FIRMware?`

Query firmware version string

- **Handler:** _scpi_cmd_query_str
- **Command ID:** CMD_SYSTEM_FIRMWARE
- **Parameters:** Query only - no parameters
- **Returns:** String value

---

### `SYSTem:HARDware?`

Query hardware revision number

- **Handler:** scpi_cmd_rw_uint8_t
- **Command ID:** AMU_REG_SYSTEM_HARDWARE_REVISION
- **Parameters:** Query only - no parameters
- **Returns:** 8-bit unsigned integer

---

### `SYSTem:LED:COLOR[?]`

Set or query LED RGB color values

- **Handler:** scpi_cmd_rw_amu_pid_t
- **Command ID:** CMD_SYSTEM_LED_COLOR
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `SYSTem:LED:PAT`

Set LED pattern mode

- **Handler:** _scpi_cmd_led
- **Command ID:** CMD_SYSTEM_LED
- **Parameters:** Command parameters vary by function
- **Returns:** Command acknowledgment

---

### `SYSTem:SCPI:LIST?`

Query system:scpi:list value

- **Handler:** scpi_cmd_execute
- **Command ID:** CMD_USB_SYSTEM_LIST_SCPI_COMMANDS
- **Parameters:** Query only - no parameters
- **Returns:** Numeric value

---

### `SYSTem:SERial?`

Query device serial number

- **Handler:** _scpi_cmd_query_str
- **Command ID:** CMD_SYSTEM_SERIAL_NUM
- **Parameters:** Query only - no parameters
- **Returns:** String value

---

### `SYSTem:SLEEP`

Enter low-power sleep mode

- **Handler:** scpi_cmd_execute
- **Command ID:** CMD_SYSTEM_SLEEP
- **Parameters:** Command parameters vary by function
- **Returns:** Command acknowledgment

---

### `SYSTem:STATus?`

Query system status flags

- **Handler:** scpi_cmd_rw_uint8_t
- **Command ID:** AMU_REG_SYSTEM_AMU_STATUS
- **Parameters:** Query only - no parameters
- **Returns:** 8-bit unsigned integer

---

### `SYSTem:TEMPerature?`

Query internal temperature in Celsius

- **Handler:** scpi_cmd_rw_float
- **Command ID:** CMD_SYSTEM_TEMPERATURE
- **Parameters:** Query only - no parameters
- **Returns:** Floating-point value

---

### `SYSTem:TIMEstamp:UTC[?]`

Set or query UTC timestamp

- **Handler:** scpi_cmd_rw_uint32_t
- **Command ID:** CMD_SYSTEM_UTC_TIME
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `SYSTem:TIMEstamp[?]`

Set or query system timestamp

- **Handler:** scpi_cmd_rw_uint32_t
- **Command ID:** CMD_SYSTEM_TIME
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `SYSTem:TWI:ADDress[?]`

Set or query I2C device address

- **Handler:** scpi_cmd_rw_uint8_t
- **Command ID:** CMD_SYSTEM_TWI_ADDRESS
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `SYSTem:TWI:MODE[?]`

Set or query I2C operating mode

- **Handler:** scpi_cmd_rw_uint8_t
- **Command ID:** CMD_USB_SYSTEM_TWI_MODE
- **Parameters:** Optional query parameter
- **Returns:** Command acknowledgment

---

### `SYSTem:TWI:NUMdevices?`

Query number of I2C devices detected

- **Handler:** scpi_cmd_rw_uint8_t
- **Command ID:** CMD_SYSTEM_TWI_NUM_DEVICES
- **Parameters:** Query only - no parameters
- **Returns:** 8-bit unsigned integer

---

### `SYSTem:TWI:SCAN?`

Scan I2C bus and return device addresses

- **Handler:** _scpi_cmd_twi_scan
- **Command ID:** CMD_USB_SYSTEM_TWI_SCAN
- **Parameters:** Query only - no parameters
- **Returns:** Numeric value

---

### `SYSTem:TWI:STATus?`

Query I2C interface status

- **Handler:** scpi_cmd_rw_uint8_t
- **Command ID:** CMD_SYSTEM_TWI_STATUS
- **Parameters:** Query only - no parameters
- **Returns:** 8-bit unsigned integer

---

### `SYSTem:VERSion?`

Query SCPI version

- **Handler:** SCPI_SystemVersionQ
- **Command ID:** 0
- **Parameters:** Query only - no parameters
- **Returns:** Numeric value

---

### `SYSTem:XMEGA:FUSES?`

Query system:xmega:fuses value

- **Handler:** scpi_cmd_rw_uint32_t
- **Command ID:** CMD_SYSTEM_XMEGA_FUSES
- **Parameters:** Query only - no parameters
- **Returns:** 32-bit unsigned integer

---

### `SYSTem:XMEGA:SIGnature?`

Query system:xmega:signature value

- **Handler:** _scpi_cmd_query_str
- **Command ID:** CMD_SYSTEM_XMEGA_USER_SIGNATURES
- **Parameters:** Query only - no parameters
- **Returns:** String value

---

