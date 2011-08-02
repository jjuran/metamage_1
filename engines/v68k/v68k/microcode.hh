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
	
	struct processor_state;
	
	
	const int max_params = 4;
	
	
	typedef void (*microcode)( processor_state&, const uint32_t* );
	
	
	void microcode_MOVEP_to  ( processor_state& state, const uint32_t* params );
	void microcode_MOVEP_from( processor_state& state, const uint32_t* params );
	
	void microcode_MOVE_B_to_Dn( processor_state& state, const uint32_t* params );
	void microcode_MOVE_B      ( processor_state& state, const uint32_t* params );
	
	void microcode_MOVE_L_to_Dn( processor_state& state, const uint32_t* params );
	void microcode_MOVEA_L     ( processor_state& state, const uint32_t* params );
	void microcode_MOVE_L      ( processor_state& state, const uint32_t* params );
	
	void microcode_MOVE_W_to_Dn( processor_state& state, const uint32_t* params );
	void microcode_MOVEA_W     ( processor_state& state, const uint32_t* params );
	void microcode_MOVE_W      ( processor_state& state, const uint32_t* params );
	
	void microcode_LEA( processor_state& state, const uint32_t* params );
	
	void microcode_BKPT( processor_state& state, const uint32_t* params );
	
	void microcode_PEA( processor_state& state, const uint32_t* params );
	
	void microcode_EXT_W( processor_state& state, const uint32_t* params );
	void microcode_EXT_L( processor_state& state, const uint32_t* params );
	
	void microcode_EXTB( processor_state& state, const uint32_t* params );
	
	void microcode_LINK( processor_state& state, const uint32_t* params );
	
	void microcode_UNLK( processor_state& state, const uint32_t* params );
	
	void microcode_MOVE_to_USP  ( processor_state& state, const uint32_t* params );
	void microcode_MOVE_from_USP( processor_state& state, const uint32_t* params );
	
	void microcode_NOP( processor_state& state, const uint32_t* params );
	
	void microcode_STOP( processor_state& state, const uint32_t* params );
	
	void microcode_RTS( processor_state& state, const uint32_t* params );
	
	void microcode_JSR( processor_state& state, const uint32_t* params );
	
	void microcode_JMP( processor_state& state, const uint32_t* params );
	
	void microcode_BRA( processor_state& state, const uint32_t* params );
	void microcode_BSR( processor_state& state, const uint32_t* params );
	void microcode_Bcc( processor_state& state, const uint32_t* params );
	
	void microcode_MOVEQ( processor_state& state, const uint32_t* params );
	
	void microcode_EXG( processor_state& state, const uint32_t* params );
	
}

#endif

