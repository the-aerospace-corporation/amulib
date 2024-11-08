/**
 * @file amu_config.h
 * @brief
 *
 * @author  CJM28241
 * @date    5/8/2019 9:06:17 PM
 */


#ifndef __AMU_CONFIG_INTERNAL_H__
#define __AMU_CONFIG_INTERNAL_H__


#if defined(__has_include) && (__has_include("amu_config.h") || __has_include("../amu_config.h"))
#	define AMU_CONFIG_MESSAGE	"Using CUSTOM amu_config.h file"
#	include "amu_config.h"
#else
#	define AMU_CONFIG_MESSAGE	"Using DEFAULT amu_config.h file"
#	define __AMU_LOW_MEMORY__
#endif

#ifdef __AMU_USE_SCPI__
#	ifdef __AMU_LOW_MEMORY__
#		define SCPI_INPUT_BUFFER_LENGTH 64
#		define SCPI_ERROR_QUEUE_SIZE 4
#	else
#		define SCPI_INPUT_BUFFER_LENGTH 1024
#		define SCPI_ERROR_QUEUE_SIZE 16
#	endif
#endif

#endif /* __AMU_CONFIG_INTERNAL_H__ */