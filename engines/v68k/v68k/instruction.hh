/*
	instruction.hh
	--------------
*/

#ifndef V68K_INSTRUCTION_HH
#define V68K_INSTRUCTION_HH

// v68k
#include "v68k/fetcher.hh"
#include "v68k/microcode.hh"


namespace v68k
{
	
	struct instruction
	{
		fetcher*   fetch;
		microcode  code;
	};
	
}

#endif

