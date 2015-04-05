/*
	line_4.hh
	---------
*/

#ifndef V68K_LINE4_HH
#define V68K_LINE4_HH

// C99
#include <stdint.h>


namespace v68k
{
	
	struct instruction;
	
	
	const instruction* decode_line_4( uint16_t opcode, instruction& storage );
	
}

#endif
