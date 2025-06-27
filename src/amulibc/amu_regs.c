#include "amu_regs.h"
#include "amu_types.h"

static volatile amu_twi_regs_t amu_twi_regs;


volatile amu_twi_regs_t* amu_regs_get_twi_regs_ptr(void) {
    return &amu_twi_regs;
}

// Macros to get size of struct members without an instance
#define MEMBER_SIZE(type, member) sizeof(((type*)0)->member)

uint16_t amu_regs_get_register_length(uint8_t reg) {

    switch(reg) {

        case AMU_REG_SYSTEM_CMD:                        return MEMBER_SIZE(amu_twi_regs_t, command);                break;
        case AMU_REG_SYSTEM_AMU_STATUS:                 return MEMBER_SIZE(amu_twi_regs_t, amu_status);             break;
        case AMU_REG_SYSTEM_TWI_STATUS:                 return MEMBER_SIZE(amu_twi_regs_t, twi_status);             break;
        case AMU_REG_SYSTEM_HARDWARE_REVISION:          return MEMBER_SIZE(amu_twi_regs_t, hardware_revision);      break;
        case AMU_REG_SYSTEM_TSENSOR_TYPE:               return MEMBER_SIZE(amu_twi_regs_t, tsensor_type);           break;
        case AMU_REG_SYSTEM_TSENSOR_NUM:                return MEMBER_SIZE(amu_twi_regs_t, tsensor_num);            break;
        case AMU_REG_SYSTEM_ADC_ACTIVE_CHANNELS:        return MEMBER_SIZE(amu_twi_regs_t, activeADCchannels);      break;
        case AMU_REG_SYSTEM_STATUS_HRADC:               return MEMBER_SIZE(amu_twi_regs_t, adc_status);             break;

        case AMU_REG_DUT_JUNCTION:                      return MEMBER_SIZE(amu_dut_t, junction);           break;
        case AMU_REG_DUT_COVERGLASS:                    return MEMBER_SIZE(amu_dut_t, coverglass);         break;
        case AMU_REG_DUT_INTERCONNECT:                  return MEMBER_SIZE(amu_dut_t, interconnect);       break;
        case AMU_REG_DUT_RESERVED:                      return MEMBER_SIZE(amu_dut_t, reserved);           break;
        case AMU_REG_DUT_MANUFACTURER:                  return MEMBER_SIZE(amu_dut_t, manufacturer);       break;
        case AMU_REG_DUT_MODEL:                         return MEMBER_SIZE(amu_dut_t, model);              break;
        case AMU_REG_DUT_TECHNOLOGY:                    return MEMBER_SIZE(amu_dut_t, technology);         break;
        case AMU_REG_DUT_SERIAL_NUMBER:                 return MEMBER_SIZE(amu_dut_t, serial);             break;
        case AMU_REG_DUT_ENERGY:                        return MEMBER_SIZE(amu_dut_t, energy);             break;
        case AMU_REG_DUT_DOSE:                          return MEMBER_SIZE(amu_dut_t, dose);               break;

        case AMU_REG_ADC_DATA_VOLTAGE:                  return MEMBER_SIZE(adc_channnels_t, val.voltage);          break;
        case AMU_REG_ADC_DATA_CURRENT:                  return MEMBER_SIZE(adc_channnels_t, val.current);          break;
        // case AMU_REG_ADC_DATA_TSENSOR:                  return MEMBER_SIZE(adc_channnels_t, val.tsensors);         break;
        // case AMU_REG_ADC_DATA_TSENSORS:                 return MEMBER_SIZE(adc_channnels_t, val.tsensors);         break;
        case AMU_REG_ADC_DATA_TSENSOR_0:                return MEMBER_SIZE(adc_channnels_t, val.tsensors[0]);      break;
        case AMU_REG_ADC_DATA_TSENSOR_1:                return MEMBER_SIZE(adc_channnels_t, val.tsensors[1]);      break;
        case AMU_REG_ADC_DATA_TSENSOR_2:                return MEMBER_SIZE(adc_channnels_t, val.tsensors[2]);      break;
        case AMU_REG_ADC_DATA_BIAS:                     return MEMBER_SIZE(adc_channnels_t, val.bias);              break;
        case AMU_REG_ADC_DATA_OFFSET:                   return MEMBER_SIZE(adc_channnels_t, val.offset);            break;
        case AMU_REG_ADC_DATA_TEMP:                     return MEMBER_SIZE(adc_channnels_t, val.adc_temp);          break;
        case AMU_REG_ADC_DATA_AVDD:                     return MEMBER_SIZE(adc_channnels_t, val.avdd);              break;
        case AMU_REG_ADC_DATA_IOVDD:                    return MEMBER_SIZE(adc_channnels_t, val.iovdd);             break;
        case AMU_REG_ADC_DATA_ALDO:                     return MEMBER_SIZE(adc_channnels_t, val.aldo);              break;
        case AMU_REG_ADC_DATA_DLDO:                     return MEMBER_SIZE(adc_channnels_t, val.dldo);              break;
        case AMU_REG_ADC_DATA_SS_TL:                    return MEMBER_SIZE(adc_channnels_t, val.ss_tl);             break;
        case AMU_REG_ADC_DATA_SS_BL:                    return MEMBER_SIZE(adc_channnels_t, val.ss_bl);             break;
        case AMU_REG_ADC_DATA_SS_BR:                    return MEMBER_SIZE(adc_channnels_t, val.ss_br);             break;
        case AMU_REG_ADC_DATA_SS_TR:                    return MEMBER_SIZE(adc_channnels_t, val.ss_tr);             break;
        
        // case AMU_REG_SUNSENSOR_TL:                      return MEMBER_SIZE(quad_photo_sensor_t, diode[0]);          break;
        // case AMU_REG_SUNSENSOR_BL:                      return MEMBER_SIZE(quad_photo_sensor_t, diode[1]);          break;
        // case AMU_REG_SUNSENSOR_BR:                      return MEMBER_SIZE(quad_photo_sensor_t, diode[2]);          break;
        // case AMU_REG_SUNSENSOR_TR:                      return MEMBER_SIZE(quad_photo_sensor_t, diode[3]);          break;
        case AMU_REG_SUNSENSOR_YAW:                     return MEMBER_SIZE(ss_angle_t, yaw);                        break;
        case AMU_REG_SUNSENSOR_PITCH:                   return MEMBER_SIZE(ss_angle_t, pitch);                      break;

        case AMU_REG_TIME_MILLIS:                       return MEMBER_SIZE(amu_twi_regs_t, milliseconds);            break;
        case AMU_REG_TIME_UTC:                          return MEMBER_SIZE(amu_twi_regs_t, utc_time);                break;

        case AMU_REG_SWEEP_CONFIG_TYPE:                 return MEMBER_SIZE(ivsweep_config_t, type);                 break;
        case AMU_REG_SWEEP_CONFIG_NUM_POINTS:           return MEMBER_SIZE(ivsweep_config_t, numPoints);            break;
        case AMU_REG_SWEEP_CONFIG_DELAY:                return MEMBER_SIZE(ivsweep_config_t, delay);                break;
        case AMU_REG_SWEEP_CONFIG_RATIO:                return MEMBER_SIZE(ivsweep_config_t, ratio);                break;
        case AMU_REG_SWEEP_CONFIG_PWR_MODE:             return MEMBER_SIZE(ivsweep_config_t, power);                break;
        case AMU_REG_SWEEP_CONFIG_DAC_GAIN:             return MEMBER_SIZE(ivsweep_config_t, dac_gain);             break;
        case AMU_REG_SWEEP_CONFIG_AVERAGES:             return MEMBER_SIZE(ivsweep_config_t, averages);             break;
        case AMU_REG_SWEEP_CONFIG_RES_1:                return MEMBER_SIZE(ivsweep_config_t, reserved);             break;
        case AMU_REG_SWEEP_CONFIG_AM0:                  return MEMBER_SIZE(ivsweep_config_t, am0);                  break;
        case AMU_REG_SWEEP_CONFIG_AREA:                 return MEMBER_SIZE(ivsweep_config_t, area);                 break;

        case AMU_REG_SWEEP_META_VOC:                    return MEMBER_SIZE(ivsweep_meta_t, voc);                    break;
        case AMU_REG_SWEEP_META_ISC:                    return MEMBER_SIZE(ivsweep_meta_t, isc);                    break;
        case AMU_REG_SWEEP_META_TSENSOR_START:          return MEMBER_SIZE(ivsweep_meta_t, tsensor_start);          break;
        case AMU_REG_SWEEP_META_TSENSOR_END:            return MEMBER_SIZE(ivsweep_meta_t, tsensor_end);            break;
        case AMU_REG_SWEEP_META_FF:                     return MEMBER_SIZE(ivsweep_meta_t, ff);                     break;
        case AMU_REG_SWEEP_META_EFF:                    return MEMBER_SIZE(ivsweep_meta_t, eff);                    break;
        case AMU_REG_SWEEP_META_VMAX:                   return MEMBER_SIZE(ivsweep_meta_t, vmax);                   break;
        case AMU_REG_SWEEP_META_IMAX:                   return MEMBER_SIZE(ivsweep_meta_t, imax);                   break;
        case AMU_REG_SWEEP_META_PMAX:                   return MEMBER_SIZE(ivsweep_meta_t, pmax);                   break;
        case AMU_REG_SWEEP_META_ADC:                    return MEMBER_SIZE(ivsweep_meta_t, adc);                    break;
        case AMU_REG_SWEEP_META_TIMESTAMP:              return MEMBER_SIZE(ivsweep_meta_t, timestamp);              break;
        case AMU_REG_SWEEP_META_CRC:                    return MEMBER_SIZE(ivsweep_meta_t, crc);                    break;

        // case AMU_REG_DATA_PTR_COMMAND:                  return MEMBER_SIZE(amu_twi_regs_t, command);                break;
        case AMU_REG_DATA_PTR_TIMESTAMP:                
        case AMU_REG_DATA_PTR_VOLTAGE:                  
        case AMU_REG_DATA_PTR_CURRENT:                  
        case AMU_REG_DATA_PTR_SS_YAW:                   
        case AMU_REG_DATA_PTR_SS_PITCH:                 return amu_twi_regs.sweep_config.numPoints * sizeof(float);

        case AMU_REG_DATA_PTR_SWEEP_CONFIG:             return sizeof(ivsweep_config_t);                            break;
        case AMU_REG_DATA_PTR_SWEEP_META:               return sizeof(ivsweep_meta_t);                              break;
        case AMU_REG_DATA_PTR_SUNSENSOR:                return sizeof(quad_photo_sensor_t);                         break;
        case AMU_REG_DATA_PTR_PRESSURE:                 return sizeof(press_data_t);                                break;

        case AMU_REG_DATA_PTR_DATAPOINT:                return sizeof(ivsweep_datapoint_t);                          break;

        default:                                        return 0;                                                    break;
    }

}