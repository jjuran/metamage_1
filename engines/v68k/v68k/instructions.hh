/*
	instructions.hh
	---------------
*/

#ifndef V68K_INSTRUCTIONS_HH
#define V68K_INSTRUCTIONS_HH

// v68k
#include "v68k/instruction.hh"


namespace v68k
{
	
	extern instruction decoded_MOVEP_to;
	extern instruction decoded_MOVEP_from;
	
	extern instruction decoded_LINK;
	extern instruction decoded_UNLK;
	
	extern instruction decoded_NOP;
	
	extern instruction decoded_MOVEQ;
	
	extern instruction decoded_EXG;
	
}

#endif

