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
	
	
	typedef void (*microcode)( processor_state&, uint32_t* );
	
	
	void microcode_ORI_to_CCR( processor_state& state, uint32_t* params );
	
	void microcode_ORI_to_SR( processor_state& state, uint32_t* params );
	
	void microcode_MOVEP_to  ( processor_state& state, uint32_t* params );
	void microcode_MOVEP_from( processor_state& state, uint32_t* params );
	
	void microcode_AND( processor_state& state, uint32_t* params );
	
	void microcode_ANDI_to_CCR( processor_state& state, uint32_t* params );
	
	void microcode_ANDI_to_SR( processor_state& state, uint32_t* params );
	
	void microcode_EORI_to_CCR( processor_state& state, uint32_t* params );
	
	void microcode_EORI_to_SR( processor_state& state, uint32_t* params );
	
	void microcode_MOVE( processor_state& state, uint32_t* params );
	
	void microcode_LEA( processor_state& state, uint32_t* params );
	
	void microcode_MOVE_from_CCR( processor_state& state, uint32_t* params );
	
	void microcode_MOVE_from_SR( processor_state& state, uint32_t* params );
	
	void microcode_MOVE_to_CCR( processor_state& state, uint32_t* params );
	
	void microcode_MOVE_to_SR( processor_state& state, uint32_t* params );
	
	void microcode_BKPT( processor_state& state, uint32_t* params );
	
	void microcode_PEA( processor_state& state, uint32_t* params );
	
	void microcode_EXT_W( processor_state& state, uint32_t* params );
	void microcode_EXT_L( processor_state& state, uint32_t* params );
	
	void microcode_EXTB( processor_state& state, uint32_t* params );
	
	void microcode_TRAP( processor_state& state, uint32_t* params );
	
	void microcode_LINK( processor_state& state, uint32_t* params );
	
	void microcode_UNLK( processor_state& state, uint32_t* params );
	
	void microcode_MOVE_to_USP  ( processor_state& state, uint32_t* params );
	void microcode_MOVE_from_USP( processor_state& state, uint32_t* params );
	
	void microcode_NOP( processor_state& state, uint32_t* params );
	
	void microcode_STOP( processor_state& state, uint32_t* params );
	
	void microcode_RTE( processor_state& state, uint32_t* params );
	
	void microcode_RTS( processor_state& state, uint32_t* params );
	
	void microcode_RTR( processor_state& state, uint32_t* params );
	
	void microcode_JSR( processor_state& state, uint32_t* params );
	
	void microcode_JMP( processor_state& state, uint32_t* params );
	
	void microcode_BRA( processor_state& state, uint32_t* params );
	void microcode_BSR( processor_state& state, uint32_t* params );
	void microcode_Bcc( processor_state& state, uint32_t* params );
	
	void microcode_EXG( processor_state& state, uint32_t* params );
	
}

#endif

