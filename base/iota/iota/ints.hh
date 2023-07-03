/*
	ints.hh
	-------
*/

#ifndef IOTA_INTS_HH
#define IOTA_INTS_HH

// Standard C
#ifndef __RELIX__
#include <stdint.h>
#endif


namespace iota
{
	
#ifdef __RELIX__
	
	typedef   signed char       int8_t;
	typedef   signed short      int16_t;
	typedef   signed long       int32_t;
	typedef   signed long long  int64_t;
	
	typedef unsigned char       uint8_t;
	typedef unsigned short      uint16_t;
	typedef unsigned long       uint32_t;
	typedef unsigned long long  uint64_t;
	
#else  // #ifdef __RELIX__
	
	using ::int8_t;
	using ::int16_t;
	using ::int32_t;
	using ::int64_t;
	
	using ::uint8_t;
	using ::uint16_t;
	using ::uint32_t;
	using ::uint64_t;
	
#endif  // #else  // #ifdef __RELIX__
	
}

#endif
