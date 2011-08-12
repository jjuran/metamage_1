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
	
	instruction decoded_ANDI_B_to_Dn =
	{
		fetches_immediate,
		microcode_AND,
		loads_and|stores_byte_data|in_register
	};
	
	instruction decoded_ANDI_B =
	{
		fetches_immediate,
		microcode_AND,
		loads_and|stores_byte_data
	};
	
	instruction decoded_ANDI_W_to_Dn =
	{
		fetches_immediate,
		microcode_AND,
		loads_and|stores_word_data|in_register
	};
	
	instruction decoded_ANDI_W =
	{
		fetches_immediate,
		microcode_AND,
		loads_and|stores_word_data
	};
	
	instruction decoded_ANDI_L_to_Dn =
	{
		fetches_immediate,
		microcode_AND,
		loads_and|stores_long_data|in_register
	};
	
	instruction decoded_ANDI_L =
	{
		fetches_immediate,
		microcode_AND,
		loads_and|stores_long_data
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
	
	instruction decoded_BKPT =
	{
		fetches_data_at_0007,
		microcode_BKPT
	};
	
	instruction decoded_PEA =
	{
		fetches_effective_control_address,
		microcode_PEA
	};
	
	instruction decoded_EXT_W =
	{
		FETCHES_MINOR_REGISTER_NUM,
		microcode_EXT_W
	};
	
	instruction decoded_EXT_L =
	{
		FETCHES_MINOR_REGISTER_NUM,
		microcode_EXT_L
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
	
	instruction decoded_RTS =
	{
		fetches_none,
		microcode_RTS
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
	
	instruction decoded_BRA_S =
	{
		fetches_branch_short,
		microcode_BRA
	};
	
	instruction decoded_BRA =
	{
		fetches_branch,
		microcode_BRA
	};
	
	instruction decoded_BRA_L =
	{
		fetches_branch_long,
		microcode_BRA,
		not_before_68020
	};
	
	instruction decoded_BSR_S =
	{
		fetches_branch_short,
		microcode_BSR
	};
	
	instruction decoded_BSR =
	{
		fetches_branch,
		microcode_BSR
	};
	
	instruction decoded_BSR_L =
	{
		fetches_branch_long,
		microcode_BSR,
		not_before_68020
	};
	
	instruction decoded_Bcc_S =
	{
		fetches_branch_short,
		microcode_Bcc
	};
	
	instruction decoded_Bcc =
	{
		fetches_branch,
		microcode_Bcc
	};
	
	instruction decoded_Bcc_L =
	{
		fetches_branch_long,
		microcode_Bcc,
		not_before_68020
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

