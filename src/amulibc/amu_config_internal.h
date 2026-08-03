/**
 * @file amu_config_internal.h
 * @brief Internal build configuration; selects a custom or default amulibc_config.h
 * 
 * @author CJM28241
 * @date 5/8/2019
 */
#ifndef __AMU_CONFIG_INTERNAL_H__
#define __AMU_CONFIG_INTERNAL_H__

#if defined(__has_include) && (__has_include("amulibc_config.h") || __has_include("../amulibc_config.h"))
#	define AMULIBC_CONFIG_MESSAGE	"Using CUSTOM amulibc_config.h file"
#	include "amulibc_config.h"
#else
#	define AMULIBC_CONFIG_MESSAGE	"Using DEFAULT amulibc_config.h file"
#	define __AMU_LOW_MEMORY__
#endif

#endif /* __AMU_CONFIG_INTERNAL_H__ */