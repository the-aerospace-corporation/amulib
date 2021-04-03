/*
 * amu_types.h
 *
 * Created: 5/5/2019 3:57:46 PM
 *  Author: colin
 */


#ifndef __AMU_TYPES_H__
#define __AMU_TYPES_H__

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>

#define AMU_ADDRESS_DEFAULT				0x0F
#define AMU_NO_ADDRESS_MATCH			0xFF

#define AMU_DEVICE_END_LIST				0xFF

#define AMU_THIS_DEVICE					0x00

#define AMU_NOTES_SIZE					128

#ifdef __AMU_LOW_MEMORY__
#define IVSWEEP_MAX_POINTS				40
#define AMU_TRANSFER_REG_SIZE			(IVSWEEP_MAX_POINTS * sizeof(float))
#else
#define IVSWEEP_MAX_POINTS				100
#define AMU_TRANSFER_REG_SIZE			(IVSWEEP_MAX_POINTS * sizeof(float) * 2)
#endif

#define AMU_DUT_MANUFACTURER_STR_LEN	16
#define AMU_DUT_MODEL_STR_LEN			16
#define AMU_DUT_TECHNOLOGY_STR_LEN		16
#define AMU_DUT_SERIALNUM_STR_LEN		24

typedef enum {
	AMU_ADC_CH_VOLTAGE = 0,
	AMU_ADC_CH_CURRENT = 1,
	AMU_ADC_CH_TSENSOR0 = 2,
	AMU_ADC_CH_TSENSOR1 = 3,
	AMU_ADC_CH_TSENSOR2 = 4,
	AMU_ADC_CH_BIAS = 5,
	AMU_ADC_CH_OFFSET = 6,
	AMU_ADC_CH_TEMP = 7,
	AMU_ADC_CH_AVDD = 8,
	AMU_ADC_CH_IOVDD = 9,
	AMU_ADC_CH_ALDO = 10,
	AMU_ADC_CH_DLDO = 11,
	AMU_ADC_CH_SS_TL = 12,
	AMU_ADC_CH_SS_BL = 13,
	AMU_ADC_CH_SS_BR = 14,
	AMU_ADC_CH_SS_TR = 15,
	AMU_ADC_CH_NUM = 16
} AMU_ADC_CH_t;

typedef enum {
	AMU_CH_EN_VOLTAGE = (1 << AMU_ADC_CH_VOLTAGE),
	AMU_CH_EN_CURRENT = (1 << AMU_ADC_CH_CURRENT),
	AMU_CH_EN_TSENSOR0 = (1 << AMU_ADC_CH_TSENSOR0),
	AMU_CH_EN_TSENSOR1 = (1 << AMU_ADC_CH_TSENSOR1),
	AMU_CH_EN_TSENSOR2 = (1 << AMU_ADC_CH_TSENSOR2),
	AMU_CH_EN_TSENSORS = (AMU_CH_EN_TSENSOR0 | AMU_CH_EN_TSENSOR1 | AMU_CH_EN_TSENSOR2),
	AMU_CH_EN_BIAS = (1 << AMU_ADC_CH_BIAS),
	AMU_CH_EN_OFFSET = (1 << AMU_ADC_CH_OFFSET),
	AMU_CH_EN_TEMP = (1 << AMU_ADC_CH_TEMP),
	AMU_CH_EN_AVDD = (1 << AMU_ADC_CH_AVDD),
	AMU_CH_EN_IOVDD = (1 << AMU_ADC_CH_IOVDD),
	AMU_CH_EN_ALDO = (1 << AMU_ADC_CH_ALDO),
	AMU_CH_EN_DLDO = (1 << AMU_ADC_CH_DLDO),
	AMU_CH_EN_INTERNAL_VOLTAGES = (AMU_CH_EN_AVDD | AMU_CH_EN_IOVDD | AMU_CH_EN_ALDO | AMU_CH_EN_DLDO),
	AMU_CH_EN_SS_TL = (1 << AMU_ADC_CH_SS_TL),
	AMU_CH_EN_SS_BL = (1 << AMU_ADC_CH_SS_BL),
	AMU_CH_EN_SS_BR = (1 << AMU_ADC_CH_SS_BR),
	AMU_CH_EN_SS_TR = (1 << AMU_ADC_CH_SS_TR),
	AMU_CH_EN_SUNSENSOR = (AMU_CH_EN_SS_TL | AMU_CH_EN_SS_BL | AMU_CH_EN_SS_BR | AMU_CH_EN_SS_TR),
	AMU_CH_EN_ALL = (0xFFFF)
} AMU_CH_EN_t;

typedef enum {
	AMU_HARDWARE_REVISION_ISC2 = 0x01,
	AMU_HARDWARE_REVISION_AMU_1_0 = 0x10,
	AMU_HARDWARE_REVISION_AMU_1_1 = 0x11,
	AMU_HARDWARE_REVISION_AMU_2_0 = 0x20,		// em version
	AMU_HARDWARE_REVISION_AMU_2_1 = 0x21,
	AMU_HARDWARE_REVISION_AMU_SP_DEV = 0x80,
	AMU_HARDWARE_REVISION_AMU_SP = 0x81,
} amu_hardware_revision_t;

typedef enum {
	AMU_TSENSOR_TYPE_PT1000_RTD = 0,
	AMU_TSENSOR_TYPE_PT100_RTD = 1,
	AMU_TSENSOR_TYPE_AD590 = 2,
} amu_tsensor_type_t;

typedef enum LED_PATTERN_enum {
	AMU_LED_PATTERN_OFF = 0,
	AMU_LED_PATTERN_WHITE_FLASH = 1,
	AMU_LED_PATTERN_QUICK_RGB_FADE = 2,
	AMU_LED_PATTERN_SLOW_RGB_FADE = 3,
	AMU_LED_PATTERN_RED_FLASH = 4,
	AMU_LED_PATTERN_GREEN_FLASH = 5,
	AMU_LED_PATTERN_BLUE_FLASH = 6,
} amu_led_pattern_t;

typedef union {
	struct {
		float A;
		float B;
		float C;
		float D;
	} val;
	float f[4];
	uint32_t i[4];
	uint8_t raw[16];
} amu_coeff_t;

typedef struct {
	float P;
	float I;
	float D;
} amu_pid_t;

typedef char amu_notes_t[AMU_NOTES_SIZE];

typedef struct {
	uint8_t type;
	uint8_t numPoints;
	uint8_t delay;
	uint8_t ratio;
	uint8_t power;
	uint8_t dac_gain;
	uint8_t averages;
	uint8_t reserved;
	float am0;
	float area;
} ivsweep_config_t;

typedef struct {
	float measurement;
	float temperature;
} amu_meas_t;

typedef struct {
	float voc;
	float isc;
	float tsensor_start;
	float tsensor_end;
	float ff;
	float eff;
	float vmax;
	float imax;
	float pmax;
	float adc;
	uint32_t timestamp;
	uint32_t crc;
} ivsweep_meta_t;

typedef struct {
	float voltage;
	float current;
} ivsweep_datapoint_t;

typedef struct {
	uint32_t timestamp[IVSWEEP_MAX_POINTS];
	float voltage[IVSWEEP_MAX_POINTS];
	float current[IVSWEEP_MAX_POINTS];
#ifndef __AMU_LOW_MEMORY__
	float yaw[IVSWEEP_MAX_POINTS];
	float pitch[IVSWEEP_MAX_POINTS];
#endif
} ivsweep_packet_t;

typedef union {
	struct {
		uint32_t voltage;
		uint32_t current;
		uint32_t tsensors[3];
		uint32_t bias;
		uint32_t offset;
		uint32_t adc_temp;
		uint32_t avdd;
		uint32_t iovdd;
		uint32_t aldo;
		uint32_t dldo;
		uint32_t ss_tl;
		uint32_t ss_bl;
		uint32_t ss_br;
		uint32_t ss_tr;
	} val;
	uint32_t channel[16];
} adc_channnels_t;

typedef struct {
	float yaw;
	float pitch;
} ss_angle_t;

typedef struct {
	float diode[4];
	ss_angle_t angle;
} quad_photo_sensor_t;

typedef struct {
	float pressure;
	float temperature;
	float humidity;
	float status;
} press_data_t;

typedef struct {
	float avdd;
	float iovdd;
	float aldo;
	float dldo;
} amu_int_volt_t;

typedef struct {
	uint8_t junction;
	uint8_t coverglass;
	uint8_t interconnect;
	uint8_t reserved;
	char manufacturer[AMU_DUT_MANUFACTURER_STR_LEN];
	char model[AMU_DUT_MODEL_STR_LEN];
	char technology[AMU_DUT_TECHNOLOGY_STR_LEN];
	char serial[AMU_DUT_SERIALNUM_STR_LEN];
	float energy;
	float dose;
} amu_dut_t;

typedef struct {
	uint8_t command;
	uint8_t amu_status;
	int8_t twi_status;
	uint8_t hardware_revision;
	uint8_t tsensor_type;
	uint8_t tsensor_num;
	uint16_t activeADCchannels;
	uint32_t hradc_status;
	amu_dut_t dut;
	adc_channnels_t adc_raw;
	ss_angle_t ss_angle;
	uint32_t utc_time;
	uint32_t milliseconds;
	ivsweep_config_t sweep_config;
	ivsweep_meta_t meta;
} amu_twi_regs_t;

typedef int8_t(*amu_transfer_fptr_t) (
	uint8_t address,		/*!< Address of AMU */
	uint8_t reg,			/*!< Register to read or write from */
	uint8_t* data,			/*!< Data pointer */
	size_t len,				/*!< Length of data to read/write */
	uint8_t read );			/*!< 1 for read, 0 for write */

typedef void(*amu_delay_fptr_t)(uint32_t period);
typedef void(*amu_watchdog_fptr_t)(void);
typedef void(*amu_watchdog_reset_fptr_t)(void);
typedef void(*amu_hardware_reset_fptr_t)(void);
typedef int(*amu_print_fptr_t)(const char* fmt, ...);
typedef uint32_t(*amu_milis_fptr_t)(void);

typedef struct {
	size_t(*write_cmd)(const char* data, size_t len);
	void(*reset_cmd)(void);
	void(*flush_cmd)(void);
} amu_scpi_dev_t;

typedef struct {

	volatile uint8_t twi_address;
	volatile amu_twi_regs_t* amu_regs;
	volatile uint8_t* transfer_reg;
	volatile ivsweep_packet_t* sweep_data;

	amu_scpi_dev_t scpi_dev;
	/*! Read function pointer */
	amu_transfer_fptr_t transfer;
	/*! Delay function pointer */
	amu_delay_fptr_t delay;
	/*! Watchdog kick function pointer */
	amu_watchdog_fptr_t watchdog_kick;
	/*! Watchdog reset function pointer, should force a watchdog timeout */
	amu_watchdog_fptr_t watchdog_reset;
	/*! Hardware reset function pointer */
	amu_hardware_reset_fptr_t hardware_reset;
	/*! Function to get current time of system in milliseconds */
	amu_milis_fptr_t millis;
	/*! Function to print errors, typically used for debugging, pass through to printf typically */
	amu_print_fptr_t print;


	/*! function to execute local commands */
	uint8_t(*process_cmd)(uint16_t cmd);

} amu_device_t;

typedef volatile uint8_t amu_data_reg_t;

#endif /* __AMU_TYPES_H__ */