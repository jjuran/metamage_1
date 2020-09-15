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
	
	enum register_t
	{
		// General registers:  0 - 15
		
		D0, D1, D2, D3, D4, D5, D6, D7,
		A0, A1, A2, A3, A4, A5, A6, A7,
		
		SP = A7,
		
		// Control registers:  16 - 31
		
		
		/*
			SFC and DFC are actually 3 bits each, but represented as 32 bits
			for convenience in MOVEC.
		*/
		SFC,
		DFC,
		
		CACR,
		
		TC,
		
		ITT0,
		ITT1,
		DTT0,
		DTT1,
		
		USP,
		
		VBR,
		
		CAAR,
		
		MSP,
		ISP,
		
		MMUSR,
		
		URP,
		SRP,
		
		// Other:  32
		
		PC,
		
		n_registers,
		
		control_register_offset = SFC,
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
