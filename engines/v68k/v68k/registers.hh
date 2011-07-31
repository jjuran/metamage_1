/*
	registers.hh
	------------
*/

#ifndef V68K_REGISTERS_HH
#define V68K_REGISTERS_HH

// C99
#include <stdint.h>


namespace v68k
{
	
	struct registers
	{
		uint32_t d[8];  // D0-D7
		uint32_t a[8];  // A0-A7
		
		uint32_t usp;
		
		uint32_t pc;
		
		uint8_t ttsm;  // T1 T0 S  M
		uint8_t  iii;  // .  I2 I1 I0
		uint8_t    x;  // .  .  .  X
		uint8_t nzvc;  // N  Z  V  C
	};
	
}

#endif

