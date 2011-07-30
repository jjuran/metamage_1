/*
	microcode.hh
	------------
*/

#ifndef V68K_MICROCODE_HH
#define V68K_MICROCODE_HH

// C99
#include <stdint.h>


namespace v68k
{
	
	struct registers;
	
	class memory;
	
	
	const int max_params = 4;
	
	
	typedef void (*microcode)( registers&, const memory&, const uint32_t* );
	
	
	void microcode_MOVEP_to  ( registers& regs, const memory& mem, const uint32_t* params );
	void microcode_MOVEP_from( registers& regs, const memory& mem, const uint32_t* params );
	
	void microcode_LINK( registers& regs, const memory& mem, const uint32_t* params );
	
	void microcode_UNLK( registers& regs, const memory& mem, const uint32_t* params );
	
	void microcode_NOP( registers& regs, const memory& mem, const uint32_t* params );
	
	void microcode_MOVEQ( registers& regs, const memory& mem, const uint32_t* params );
	
	void microcode_EXG( registers& regs, const memory& mem, const uint32_t* params );
	
}

#endif

