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
		// General registers:  0 - 15
		
		uint32_t d[8];  // D0-D7
		uint32_t a[8];  // A0-A7
		
		// Control registers:  16 - 31
		
		/*
			SFC and DFC are actually 3 bits each, but represented as 32 bits
			for convenience in MOVEC.
		*/
		uint32_t sfc;
		uint32_t dfc;
		
		uint32_t cacr;
		
		uint32_t tc;
		
		uint32_t itt0;
		uint32_t itt1;
		uint32_t dtt0;
		uint32_t dtt1;
		
		uint32_t usp;
		
		uint32_t vbr;
		
		uint32_t caar;
		
		uint32_t msp;
		uint32_t isp;
		
		uint32_t mmusr;
		
		uint32_t urp;
		uint32_t srp;
		
		// Other:  32
		
		uint32_t pc;
	};
	
	struct status_register
	{
		uint8_t ttsm;  // T1 T0 S  M
		uint8_t  iii;  // .  I2 I1 I0
		uint8_t    x;  // .  .  .  X
		uint8_t nzvc;  // N  Z  V  C
	};
	
}

#endif

