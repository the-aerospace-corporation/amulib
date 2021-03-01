/**
 * @file amu_config.h
 * @brief
 *
 * @author  CJM28241
 * @date    5/8/2019 9:06:17 PM
 */


#ifndef AMU_CONFIG_INTERNAL_H_
#define AMU_CONFIG_INTERNAL_H_

#include <stdint.h>

#ifdef CONFIG_AMU_CONFIG_SKIP
#define AMU_CONFIG_SKIP
#endif

/* If "amu_config.h" is available from here try to use it later.*/
#if defined __has_include
#  if __has_include("amu_config.h")
#   ifndef AMU_CONFIG_INCLUDE_SIMPLE
#    define AMU_CONFIG_INCLUDE_SIMPLE
#   endif
#  endif
#endif

/*If amu_config.h is not skipped include it*/
#if !defined(AMU_CONFIG_SKIP)
#  if defined(AMU_CONFIG_PATH)											/*If there is a path defined for amu_config.h use it*/
#    define __LV_TO_STR_AUX(x) #x
#    define __LV_TO_STR(x) __LV_TO_STR_AUX(x)
#    include __LV_TO_STR(AMU_CONFIG_PATH)
#    undef __LV_TO_STR_AUX
#    undef __LV_TO_STR
#  elif defined(AMU_CONFIG_INCLUDE_SIMPLE)        /*Or simply include amu_config.h is enabled*/
#    include "amu_config.h"
#  else
#    include "../amu_config.h"                 /*Else assume amu_config.h is in the src folder */
#  endif
#endif

#endif /* AMU_CONFIG_INTERNAL_H_ */