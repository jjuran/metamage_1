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
	
	struct registers;
	
	class memory;
	
	
	const instruction* decode( const registers& regs, const memory& mem );
	
}

#endif

