/*
	instructions.cc
	---------------
*/

#include "v68k/instructions.hh"

// v68k
#include "v68k/fetches.hh"
#include "v68k/microcode.hh"


#define FETCHES_MINOR_REGISTER_NUM  fetches_data_at_0007


namespace v68k
{
	
	instruction decoded_ORI_to_CCR =
	{
		fetches_unsigned_word,
		microcode_ORI_to_CCR
	};
	
	instruction decoded_ORI_to_SR =
	{
		fetches_unsigned_word,
		microcode_ORI_to_SR,
		privileged
	};
	
	instruction decoded_MOVEP_to =
	{
		fetches_MOVEP,
		microcode_MOVEP_to
	};
	
	instruction decoded_MOVEP_from =
	{
		fetches_MOVEP,
		microcode_MOVEP_from
	};
	
	instruction decoded_ANDI_to_CCR =
	{
		fetches_unsigned_word,
		microcode_ANDI_to_CCR
	};
	
	instruction decoded_ANDI_to_SR =
	{
		fetches_unsigned_word,
		microcode_ANDI_to_SR,
		privileged
	};
	
	instruction decoded_EORI_to_CCR =
	{
		fetches_unsigned_word,
		microcode_EORI_to_CCR
	};
	
	instruction decoded_EORI_to_SR =
	{
		fetches_unsigned_word,
		microcode_EORI_to_SR,
		privileged
	};
	
	instruction decoded_MOVES =
	{
		fetches_MOVES,
		microcode_MOVES,
		privileged | not_before_68010
	};
	
	instruction decoded_MOVE_B_to_Dn =
	{
		fetches_MOVE_B_to_Dn,
		microcode_MOVE,
		stores_byte_data|in_register
	};
	
	instruction decoded_MOVE_B =
	{
		fetches_MOVE_B,
		microcode_MOVE,
		stores_byte_data
	};
	
	instruction decoded_MOVE_L_to_Rn =
	{
		fetches_MOVE_L_to_Rn,
		microcode_MOVE,
		stores_long_data|in_register
	};
	
	instruction decoded_MOVE_L =
	{
		fetches_MOVE_L,
		microcode_MOVE,
		stores_long_data
	};
	
	instruction decoded_MOVE_W_to_Rn =
	{
		fetches_MOVE_W_to_Rn,
		microcode_MOVE,
		stores_word_data|in_register
	};
	
	instruction decoded_MOVE_W =
	{
		fetches_MOVE_W,
		microcode_MOVE,
		stores_word_data
	};
	
	instruction decoded_LINK_L =
	{
		fetches_LINK_L,
		microcode_LINK,  // same as LINK
		not_before_68020
	};
	
	instruction decoded_LEA =
	{
		fetches_LEA,
		microcode_LEA
	};
	
	instruction decoded_MOVE_from_CCR_to_Dn =
	{
		fetches_MOVE_from_SR,
		microcode_MOVE_from_CCR,
		not_before_68010 | stores_word_data|in_register
	};
	
	instruction decoded_MOVE_from_CCR =
	{
		fetches_MOVE_from_SR,
		microcode_MOVE_from_CCR,
		not_before_68010 | stores_word_data
	};
	
	instruction decoded_MOVE_from_SR_to_Dn =
	{
		fetches_MOVE_from_SR,
		microcode_MOVE_from_SR,
		privileged_except_on_68000 | stores_word_data|in_register
	};
	
	instruction decoded_MOVE_from_SR =
	{
		fetches_MOVE_from_SR,
		microcode_MOVE_from_SR,
		privileged_except_on_68000 | stores_word_data
	};
	
	instruction decoded_MOVE_to_CCR =
	{
		fetches_MOVE_to_SR,
		microcode_MOVE_to_CCR
	};
	
	instruction decoded_MOVE_to_SR =
	{
		fetches_MOVE_to_SR,
		microcode_MOVE_to_SR,
		privileged
	};
	
	instruction decoded_PEA =
	{
		fetches_effective_control_address,
		microcode_PEA
	};
	
	instruction decoded_EXTB =
	{
		FETCHES_MINOR_REGISTER_NUM,
		microcode_EXTB,
		not_before_68020
	};
	
	instruction decoded_TRAP =
	{
		fetches_TRAP,
		microcode_TRAP
	};
	
	instruction decoded_LINK =
	{
		fetches_LINK,
		microcode_LINK
	};
	
	instruction decoded_UNLK =
	{
		FETCHES_MINOR_REGISTER_NUM,
		microcode_UNLK
	};
	
	instruction decoded_MOVE_to_USP =
	{
		FETCHES_MINOR_REGISTER_NUM,
		microcode_MOVE_to_USP,
		privileged
	};
	
	instruction decoded_MOVE_from_USP =
	{
		FETCHES_MINOR_REGISTER_NUM,
		microcode_MOVE_from_USP,
		privileged
	};
	
	instruction decoded_RESET =
	{
		fetches_none,
		microcode_NOP,
		privileged
	};
	
	instruction decoded_NOP =
	{
		fetches_none,
		microcode_NOP
	};
	
	instruction decoded_STOP =
	{
		fetches_unsigned_word,
		microcode_STOP,
		privileged
	};
	
	instruction decoded_RTE =
	{
		fetches_none,
		microcode_RTE,
		privileged
	};
	
	instruction decoded_RTD =
	{
		fetches_signed_word,
		microcode_RTD,
		not_before_68010
	};
	
	instruction decoded_RTS =
	{
		fetches_none,
		microcode_RTS
	};
	
	instruction decoded_TRAPV =
	{
		fetches_none,
		microcode_TRAPV
	};
	
	instruction decoded_RTR =
	{
		fetches_none,
		microcode_RTR
	};
	
	instruction decoded_MOVEC =
	{
		fetches_unsigned_word,
		microcode_MOVEC,
		privileged | not_before_68010
	};
	
	instruction decoded_JSR =
	{
		fetches_effective_control_address,
		microcode_JSR
	};
	
	instruction decoded_JMP =
	{
		fetches_effective_control_address,
		microcode_JMP
	};
	
	instruction decoded_MOVEQ =
	{
		fetches_MOVEQ,
		microcode_MOVE,
		stores_long_data|in_register
	};
	
	instruction decoded_EXG =
	{
		fetches_EXG,
		microcode_EXG
	};
	
}

