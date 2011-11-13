/*
	macros.hh
	---------
*/

#ifndef V68K_MACROS_HH
#define V68K_MACROS_HH

// C99
#include <stdint.h>

// v68k
#include "v68k/instruction.hh"


namespace v68k
{
	
	int32_t sign_extend( int32_t data, op_size_t size );
	
}

#endif

