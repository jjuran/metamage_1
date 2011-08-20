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
		
		uint32_t alt_sp;   // whichever of USP or SSP is not A7
		uint32_t alt_ssp;  // whichever of ISP or MSP is not SSP
		
		uint32_t pc;
		
		/*
			SFC and DFC are actually 3 bits each, but represented as 32 bits
			for convenience in MOVEC.
		*/
		uint32_t sfc;
		uint32_t dfc;
		
		uint32_t vbr;
		
		uint8_t ttsm;  // T1 T0 S  M
		uint8_t  iii;  // .  I2 I1 I0
		uint8_t    x;  // .  .  .  X
		uint8_t nzvc;  // N  Z  V  C
	};
	
}

#endif

