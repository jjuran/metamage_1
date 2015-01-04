/*
	decode.hh
	---------
*/

#ifndef V68K_DECODE_HH
#define V68K_DECODE_HH

// C99
#include <stdint.h>


namespace v68k
{
	
	struct instruction;
	
	
	const instruction* decode( uint16_t opcode, instruction& storage );
	
}

#endif
