/*-
 * Copyright (c) 2012-2013 Jan Breuer,
 *
 * All Rights Reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file   cc.h
 *
 * @brief  compiler detection
 *
 *
 */

#ifndef __SCPI_CC_H_
#define __SCPI_CC_H_

#ifdef	__cplusplus
extern "C" {
#endif

#if defined(__STDC__)
# define C89 1
# if defined(__STDC_VERSION__)
#  define C90 1
#  if (__STDC_VERSION__ >= 199409L)
#   define C94 1
#  endif
#  if (__STDC_VERSION__ >= 199901L)
#   define C99 1
#  endif
# endif
#endif

#if _POSIX_C_SOURCE >= 200809L || _XOPEN_SOURCE >= 700 || __cplusplus >= 200112L
    #define HAVE_STRNDUP 1
    #define HAVE_STRNLEN 1
#endif

#if _BSD_SOURCE || _XOPEN_SOURCE >= 500 || _ISOC99_SOURCE || _POSIX_C_SOURCE >= 200112L || C99
    #define HAVE_SNPRINTF 0
#endif

#if _POSIX_C_SOURCE >= 200112L
    #define HAVE_STRNCASECMP 1
#endif

#if _BSD_SOURCE || _SVID_SOURCE || _XOPEN_SOURCE || _ISOC99_SOURCE || _POSIX_C_SOURCE >= 200112L || C99
    #define HAVE_ISNAN 1
#endif

#if _XOPEN_SOURCE >= 600 || _ISOC99_SOURCE || _POSIX_C_SOURCE >= 200112L || C99
    #define HAVE_ISFINITE 1
    #define HAVE_SIGNBIT 1
#endif

#if XOPEN_SOURCE >= 600 || _BSD_SOURCE || _SVID_SOURCE || _ISOC99_SOURCE || _POSIX_C_SOURCE >= 200112L
    #define HAVE_STRTOLL 1
#endif

#if _XOPEN_SOURCE >= 600 || _ISOC99_SOURCE || _POSIX_C_SOURCE >= 200112L || C99
    #define HAVE_STRTOF 1
#endif

#if _ISOC99_SOURCE || C99
    #define HAVE_STDBOOL 1
#endif

/* Compiler specific */
/* RealView/Keil ARM Compiler, e.g. Cortex-M CPUs */
#if defined(__CC_ARM)
#define HAVE_STRNCASECMP        1
#endif

/* National Instruments (R) CVI x86/x64 PC platform */
#if defined(_CVI_)
#define HAVE_STRNICMP           1
#endif

/* 8bit PIC - PIC16, etc */
#if defined(_MPC_)
#define HAVE_STRNICMP           1
#endif

/* PIC24 */
#if defined(__C30__)
#endif

/* PIC32mx */
#if defined(__C32__)
#define HAVE_FINITE             1
#endif

/* AVR libc */
#if defined(__AVR__)
#include <stdlib.h>
#define HAVE_DTOSTRE            1
#undef HAVE_STRTOF
#define HAVE_STRTOF				0
#define HAVE_STDBOOL			1
#endif

/* Teensy */
#if defined(TEENSYDUINO)
#include <stdlib.h>
#define HAVE_DTOSTRE            0
#define HAVE_STRTOF				1
#define HAVE_STDBOOL			1
#endif
    
    
/* ARM */
#if defined(__arm__)
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
    
#ifndef HAVE_SNPRINTF
#define HAVE_SNPRINTF           1
#endif

#ifndef HAVE_STDBOOL
#define HAVE_STDBOOL			1
#endif
    
#ifndef HAVE_ISNAN
#define HAVE_ISNAN              1
#endif

#ifndef HAVE_ISFINITE
#define HAVE_ISFINITE           1
#endif
    
#ifndef HAVE_FINITE
#define HAVE_FINITE             1
#endif
    
#ifndef HAVE_SIGNBIT
#define HAVE_SIGNBIT            1
#endif
    
#ifndef HAVE_STRTOF
#define HAVE_STRTOF             1
#endif
    
#ifndef HAVE_STRTOLL
#define HAVE_STRTOLL            1
#endif

#endif

#if defined(__SAMD21E18A__)
#include <stdlib.h>
#define HAVE_DTOSTRE            0
#define HAVE_STRTOF				1
#define HAVE_STDBOOL			1
#define HAVE_STRNLEN            1
#endif

/* default values */
#ifndef HAVE_STRNLEN
#define HAVE_STRNLEN            0
#endif

#ifndef HAVE_STRDUP
#define HAVE_STRDUP             0
#endif

#ifndef HAVE_STRNICMP
#define HAVE_STRNICMP           0
#endif

#ifndef HAVE_STDBOOL
#define HAVE_STDBOOL            0
#endif

#ifndef HAVE_SNPRINTF
#define HAVE_SNPRINTF           0
#endif

#ifndef HAVE_STRNCASECMP
#define HAVE_STRNCASECMP        0
#endif

#ifndef HAVE_ISNAN
#define HAVE_ISNAN              0
#endif

#ifndef HAVE_ISFINITE
#define HAVE_ISFINITE           0
#endif

#ifndef HAVE_FINITE
#define HAVE_FINITE             0
#endif

#ifndef HAVE_SIGNBIT
#define HAVE_SIGNBIT            0
#endif

#ifndef HAVE_STRTOLL
#define HAVE_STRTOLL            0
#endif

#ifndef HAVE_STRTOF
#define HAVE_STRTOF             0
#endif

#ifdef	__cplusplus
}
#endif

#endif /* __SCPI_CC_H_ */