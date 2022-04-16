/*
	microcode.hh
	------------
*/

#ifndef V68K_MICROCODE_HH
#define V68K_MICROCODE_HH

// v68k
#include "v68k/op.hh"
#include "v68k/op_params.hh"


namespace v68k
{
	
	struct processor_state;
	
	
	typedef op_result (*microcode)( processor_state&, op_params& );
	
	
	#pragma mark -
	#pragma mark Line 0
	
	op_result microcode_ORI_to_CCR( processor_state& state, op_params& pb );
	
	op_result microcode_ORI_to_SR( processor_state& state, op_params& pb );
	
	op_result microcode_BCHG( processor_state& state, op_params& pb );
	op_result microcode_BCLR( processor_state& state, op_params& pb );
	op_result microcode_BSET( processor_state& state, op_params& pb );
	
	op_result microcode_MOVEP_to  ( processor_state& state, op_params& pb );
	op_result microcode_MOVEP_from( processor_state& state, op_params& pb );
	
	op_result microcode_ANDI_to_CCR( processor_state& state, op_params& pb );
	
	op_result microcode_ANDI_to_SR( processor_state& state, op_params& pb );
	
	op_result microcode_EORI_to_CCR( processor_state& state, op_params& pb );
	
	op_result microcode_EORI_to_SR( processor_state& state, op_params& pb );
	
	op_result microcode_MOVES( processor_state& state, op_params& pb );
	
	#pragma mark -
	#pragma mark Lines 1-3
	
	op_result microcode_MOVE( processor_state& state, op_params& pb );
	
	#pragma mark -
	#pragma mark Line 4
	
	op_result microcode_CHK( processor_state& state, op_params& pb );
	
	op_result microcode_LEA( processor_state& state, op_params& pb );
	
	op_result microcode_MOVE_from_CCR( processor_state& state, op_params& pb );
	
	op_result microcode_MOVE_from_SR( processor_state& state, op_params& pb );
	
	op_result microcode_MOVE_to_CCR( processor_state& state, op_params& pb );
	
	op_result microcode_MOVE_to_SR( processor_state& state, op_params& pb );
	
	op_result microcode_SWAP( processor_state& state, op_params& pb );
	
	op_result microcode_BKPT( processor_state& state, op_params& pb );
	
	op_result microcode_PEA( processor_state& state, op_params& pb );
	
	op_result microcode_TAS( processor_state& state, op_params& pb );
	
	op_result microcode_MUL_L( processor_state& s, op_params& pb );
	op_result microcode_DIV_L( processor_state& s, op_params& pb );
	
	op_result microcode_MOVEM_to  ( processor_state& state, op_params& pb );
	op_result microcode_MOVEM_from( processor_state& state, op_params& pb );
	
	op_result microcode_TRAP( processor_state& state, op_params& pb );
	
	op_result microcode_LINK( processor_state& state, op_params& pb );
	
	op_result microcode_UNLK( processor_state& state, op_params& pb );
	
	op_result microcode_MOVE_to_USP  ( processor_state& state, op_params& pb );
	op_result microcode_MOVE_from_USP( processor_state& state, op_params& pb );
	
	op_result microcode_NOP( processor_state& state, op_params& pb );
	
	op_result microcode_STOP( processor_state& state, op_params& pb );
	
	op_result microcode_RTE( processor_state& state, op_params& pb );
	
	op_result microcode_RTD( processor_state& state, op_params& pb );
	
	op_result microcode_RTS( processor_state& state, op_params& pb );
	
	op_result microcode_TRAPV( processor_state& state, op_params& pb );
	
	op_result microcode_RTR( processor_state& state, op_params& pb );
	
	op_result microcode_MOVEC( processor_state& state, op_params& pb );
	
	#pragma mark -
	#pragma mark Line 5
	
	op_result microcode_DBcc( processor_state& state, op_params& pb );
	
	#pragma mark -
	#pragma mark Line 6
	
	op_result microcode_BRA( processor_state& state, op_params& pb );
	op_result microcode_BSR( processor_state& state, op_params& pb );
	op_result microcode_Bcc( processor_state& state, op_params& pb );
	
	#pragma mark -
	#pragma mark Line 8
	
	op_result microcode_OR( processor_state& state, op_params& pb );
	
	op_result microcode_SBCD( processor_state& state, op_params& pb );
	
	op_result microcode_DIVS( processor_state& state, op_params& pb );
	
	op_result microcode_DIVU( processor_state& state, op_params& pb );
	
	#pragma mark -
	#pragma mark Line 9
	
	op_result microcode_SUB( processor_state& state, op_params& pb );
	
	#pragma mark -
	#pragma mark Line B
	
	op_result microcode_EOR( processor_state& state, op_params& pb );
	
	#pragma mark -
	#pragma mark Line C
	
	op_result microcode_AND( processor_state& state, op_params& pb );
	
	op_result microcode_EXG( processor_state& state, op_params& pb );
	
	op_result microcode_ABCD( processor_state& state, op_params& pb );
	
	op_result microcode_MULS( processor_state& state, op_params& pb );
	
	op_result microcode_MULU( processor_state& state, op_params& pb );
	
	#pragma mark -
	#pragma mark Line D
	
	op_result microcode_ADD( processor_state& state, op_params& pb );
	
	#pragma mark -
	#pragma mark Line E
	
	op_result microcode_ASR( processor_state& state, op_params& pb );
	op_result microcode_ASL( processor_state& state, op_params& pb );
	
	op_result microcode_LSR( processor_state& state, op_params& pb );
	op_result microcode_LSL( processor_state& state, op_params& pb );
	
	op_result microcode_ROXR( processor_state& state, op_params& pb );
	op_result microcode_ROXL( processor_state& state, op_params& pb );
	
	op_result microcode_ROR( processor_state& state, op_params& pb );
	op_result microcode_ROL( processor_state& state, op_params& pb );
	
}

#endif
