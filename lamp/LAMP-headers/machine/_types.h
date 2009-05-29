#ifndef _MACH_TYPES_H_
#define _MACH_TYPES_H_

#include <size_t.h>
#include <va_list.h>

#define	_SIZE_T_DEFINED_

#ifdef __cplusplus
using std::size_t;
using std::va_list;
#endif
typedef va_list __va_list;

typedef   signed char       __int8_t;
typedef   signed short      __int16_t;
typedef   signed int        __int32_t;
typedef   signed long long  __int64_t;

typedef unsigned char       __uint8_t;
typedef unsigned short      __uint16_t;
typedef unsigned int        __uint32_t;
typedef unsigned long long  __uint64_t;

typedef __uint64_t __clock_t;

typedef long __time_t;

typedef long __off_t;

typedef          long  __ssize_t;
typedef unsigned long  __size_t;

typedef int __clockid_t;

typedef unsigned short __wchar_t;

#ifdef __MC68K__

#endif

#ifdef __POWERPC__

#endif

#endif

