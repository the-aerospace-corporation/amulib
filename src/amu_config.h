/**
 * @file amu_config.h
 * @brief
 *
 * @author  CJM28241
 * @date    5/8/2019 9:06:17 PM
 */


#ifndef AMU_CONFIG_H_
#define AMU_CONFIG_H_

#define __AMU_DEVICE__

#define __AMU_USE_SCPI__

//#define AMU_LOW_MEMORY

//#define AMU_XMEGA

/** @name SCPI */
#ifdef __AMU_USE_SCPI__
#define SCPI_INPUT_BUFFER_LENGTH 1024
#define SCPI_ERROR_QUEUE_SIZE 16
#ifdef AMU_XMEGA
#define SCPI_USE_PROGMEM
#endif
#endif

#endif /* AMU_CONFIG_H_ */