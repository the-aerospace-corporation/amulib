# SCPI Command Reference

This document provides a comprehensive reference for all SCPI commands supported by the AMU library.
Documentation is extracted from the command enum definitions in `amu_commands.h`.

## System Commands

### `SYSTem:BOOTloader`

**Description:** Enters bootloader mode for firmware updates (USB only)

**Enum:** `CMD_USB_SYSTEM_ENTER_BOOTLOADER`

**Type:** Set/Get

---

### `SYSTem:CONFig:CURRent:GAIN[?]`

**Description:** Sets or queries current measurement amplifier gain (USB only)

**Enum:** `CMD_USB_SYSTEM_CONFIG_CURR_GAIN`

**Type:** Query

---

### `SYSTem:CONFig:CURRent:Rsense[?]`

**Description:** Sets or queries current sense resistor value in ohms (USB only)

**Enum:** `CMD_USB_SYSTEM_CONFIG_CURR_RSENSE`

**Type:** Query

---

### `SYSTem:CONFig:SAVE`

**Description:** Saves current system configuration to EEPROM (USB only)

**Enum:** `CMD_USB_SYSTEM_CONFIG_SAVE`

**Type:** Set/Get

---

### `SYSTem:CONFig:VOLTage:R1[?]`

**Description:** Sets or queries voltage divider R1 resistor value in ohms (USB only)

**Enum:** `CMD_USB_SYSTEM_CONFIG_VOLT_R1`

**Type:** Query

---

### `SYSTem:CONFig:VOLTage:R2[?]`

**Description:** Sets or queries voltage divider R2 resistor value in ohms (USB only)

**Enum:** `CMD_USB_SYSTEM_CONFIG_VOLT_R2`

**Type:** Query

---

### `SYSTem:DEBug#?`

**Description:** Sets or queries debug values for development (USB only)

**Enum:** `CMD_USB_SYSTEM_DEBUG`

**Type:** Query

---

### `SYSTem:SCPI:LIST?`

**Description:** Returns list of all supported SCPI commands (USB only)

**Enum:** `CMD_USB_SYSTEM_LIST_SCPI_COMMANDS`

**Type:** Query

---

### `SYSTem:TWI:MODE[?]`

**Description:** Sets or queries I2C operating mode (USB only)

**Enum:** `CMD_USB_SYSTEM_TWI_MODE`

**Type:** Query

---

### `SYSTem:TWI:SCAN?`

**Description:** Scans I2C bus and returns list of device addresses (USB only)

**Enum:** `CMD_USB_SYSTEM_TWI_SCAN`

**Type:** Query

---

## Memory Commands

### `MEMory:ADC:CH#:GAIN[?]`

**Description:** Sets or queries EEPROM-stored ADC channel gain calibration (USB only)

**Enum:** `CMD_USB_EEPROM_GAIN`

**Type:** Query

---

### `MEMory:ADC:CH#:OFFset[?]`

**Description:** Sets or queries EEPROM-stored ADC channel offset calibration (USB only)

**Enum:** `CMD_USB_EEPROM_OFFSET`

**Type:** Query

---

### `MEMory:CURRent:GAIN#[?]`

**Description:** Sets or queries EEPROM-stored current gain calibration (USB only)

**Enum:** `CMD_USB_EEPROM_CURRENT_GAIN`

**Type:** Query

---

### `MEMory:CURRent:OFFset#[?]`

**Description:** Sets or queries EEPROM-stored current offset calibration (USB only)

**Enum:** `CMD_USB_EEPROM_CURRENT_OFFSET`

**Type:** Query

---

### `MEMory:ERASE:ALL`

**Description:** Erases all user data from EEPROM (USB only)

**Enum:** `CMD_USB_EEPROM_ERASE_ALL`

**Type:** Set/Get

---

### `MEMory:ERASE:CONFig`

**Description:** Erases configuration data from EEPROM (USB only)

**Enum:** `CMD_USB_EEPROM_ERASE_CONFIG`

**Type:** Set/Get

---

### `MEMory:VOLTage:GAIN#[?]`

**Description:** Sets or queries EEPROM-stored voltage gain calibration (USB only)

**Enum:** `CMD_USB_EEPROM_VOLTAGE_GAIN`

**Type:** Query

---

### `MEMory:VOLTage:OFFset#[?]`

**Description:** Sets or queries EEPROM-stored voltage offset calibration (USB only)

**Enum:** `CMD_USB_EEPROM_VOLTAGE_OFFSET`

**Type:** Query

---

## ADC Commands

### `ADC:CURRent:CALibrate:FULL`

**Description:** Performs full-scale calibration for current channel (USB only)

**Enum:** `CMD_USB_ADC_CURRENT_CAL_FULL`

**Type:** Set/Get

---

### `ADC:CURRent:CALibrate:RESet`

**Description:** Resets current channel calibration to defaults (USB only)

**Enum:** `CMD_USB_ADC_CURRENT_CAL_RESET`

**Type:** Set/Get

---

### `ADC:CURRent:CALibrate:SAVe`

**Description:** Saves current channel calibration to EEPROM (USB only)

**Enum:** `CMD_USB_ADC_CURRENT_CAL_SAVE`

**Type:** Set/Get

---

### `ADC:CURRent:CALibrate:ZERO`

**Description:** Performs zero-scale calibration for current channel (USB only)

**Enum:** `CMD_USB_ADC_CURRENT_CAL_ZERO`

**Type:** Set/Get

---

### `ADC:CURRent:GAIN[?]`

**Description:** Sets or queries current channel gain coefficient (USB only)

**Enum:** `CMD_USB_ADC_CURRENT_GAIN`

**Type:** Query

---

### `ADC:CURRent:MAX?`

**Description:** Queries current channel maximum input range (USB only)

**Enum:** `CMD_USB_ADC_CURRENT_MAX`

**Type:** Query

---

### `ADC:CURRent:OFFset[?]`

**Description:** Sets or queries current channel offset coefficient (USB only)

**Enum:** `CMD_USB_ADC_CURRENT_OFFSET`

**Type:** Query

---

### `ADC:CURRent:PGA[?]`

**Description:** Sets or queries current channel PGA setting (USB only)

**Enum:** `CMD_USB_ADC_CURRENT_PGA`

**Type:** Query

---

### `ADC:VOLTage:CALibrate:FULL`

**Description:** Performs full-scale calibration for voltage channel (USB only)

**Enum:** `CMD_USB_ADC_VOLTAGE_CAL_FULL`

**Type:** Set/Get

---

### `ADC:VOLTage:CALibrate:RESet`

**Description:** Resets voltage channel calibration to defaults (USB only)

**Enum:** `CMD_USB_ADC_VOLTAGE_CAL_RESET`

**Type:** Set/Get

---

### `ADC:VOLTage:CALibrate:SAVe`

**Description:** Saves voltage channel calibration to EEPROM (USB only)

**Enum:** `CMD_USB_ADC_VOLTAGE_CAL_SAVE`

**Type:** Set/Get

---

### `ADC:VOLTage:CALibrate:ZERO`

**Description:** Performs zero-scale calibration for voltage channel (USB only)

**Enum:** `CMD_USB_ADC_VOLTAGE_CAL_ZERO`

**Type:** Set/Get

---

### `ADC:VOLTage:GAIN[?]`

**Description:** Sets or queries voltage channel gain coefficient (USB only)

**Enum:** `CMD_USB_ADC_VOLTAGE_GAIN`

**Type:** Query

---

### `ADC:VOLTage:MAX?`

**Description:** Queries voltage channel maximum input range (USB only)

**Enum:** `CMD_USB_ADC_VOLTAGE_MAX`

**Type:** Query

---

### `ADC:VOLTage:OFFset[?]`

**Description:** Sets or queries voltage channel offset coefficient (USB only)

**Enum:** `CMD_USB_ADC_VOLTAGE_OFFSET`

**Type:** Query

---

### `ADC:VOLTage:PGA[?]`

**Description:** Sets or queries voltage channel PGA setting (USB only)

**Enum:** `CMD_USB_ADC_VOLTAGE_PGA`

**Type:** Query

---

## Sweep Commands

### `SWEEP:TRIGger:USB`

**Description:** Triggers I-V sweep with USB-specific parameters (USB only)

**Enum:** `CMD_USB_SWEEP_TRIGGER`

**Type:** Set/Get

---
