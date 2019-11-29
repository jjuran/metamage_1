/*
	instructions.cc
	---------------
*/

#include "v68k/instructions.hh"

// v68k
#include "v68k/fetches.hh"
#include "v68k/microcode.hh"
#include "v68k/ops.hh"


#define FETCHES_MINOR_REGISTER_NUM  fetches_data_at_0007


namespace v68k
{
	
	instruction decoded_MOVES =
	{
		fetches_word_and_EA,
		microcode_MOVES,
		op_size_in_00C0,
		privileged | not_before_68010
	};
	
	instruction decoded_MOVE_B =
	{
		fetches_MOVE,
		microcode_MOVE,
		byte_sized,
		stores_data | basic_CCR_update
	};
	
	instruction decoded_MOVE_L =
	{
		fetches_MOVE,
		microcode_MOVE,
		long_sized,
		stores_data | basic_CCR_update
	};
	
	instruction decoded_MOVE_W =
	{
		fetches_MOVE,
		microcode_MOVE,
		word_sized,
		stores_data | basic_CCR_update
	};
	
	instruction decoded_LINK_L =
	{
		fetches_LINK,
		microcode_LINK,  // same as LINK
		long_sized,
		not_before_68020
	};
	
	instruction decoded_LEA =
	{
		fetches_LEA,
		microcode_LEA
	};
	
	instruction decoded_MOVE_to_CCR =
	{
		fetches_MOVE_to_SR,
		microcode_MOVE_to_CCR,
		word_sized
	};
	
	instruction decoded_MOVE_to_SR =
	{
		fetches_MOVE_to_SR,
		microcode_MOVE_to_SR,
		word_sized,
		privileged
	};
	
	instruction decoded_PEA =
	{
		fetches_effective_address,
		microcode_PEA,
		unsized,
		uses_stack,
	};
	
	instruction decoded_EXTB =
	{
		FETCHES_MINOR_REGISTER_NUM,
		sign_extend_byte,
		long_sized,
		not_before_68020 | loads_and | stores_data | basic_CCR_update
	};
	
	instruction decoded_TRAP =
	{
		fetches_TRAP,
		microcode_TRAP
	};
	
	instruction decoded_LINK =
	{
		fetches_LINK,
		microcode_LINK,
		word_sized,
		uses_stack,
	};
	
	instruction decoded_UNLK =
	{
		fetches_UNLK,
		microcode_UNLK,
		unsized,
		uses_stack,
	};
	
	instruction decoded_MOVE_to_USP =
	{
		FETCHES_MINOR_REGISTER_NUM,
		microcode_MOVE_to_USP,
		long_sized,
		privileged
	};
	
	instruction decoded_MOVE_from_USP =
	{
		FETCHES_MINOR_REGISTER_NUM,
		microcode_MOVE_from_USP,
		long_sized,
		privileged
	};
	
	instruction decoded_RESET =
	{
		fetches_none,
		microcode_NOP,
		unsized,
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
		word_sized,
		privileged
	};
	
	instruction decoded_RTE =
	{
		fetches_none,
		microcode_RTE,
		unsized,
		privileged | uses_stack,
	};
	
	instruction decoded_RTD =
	{
		fetches_signed_word,
		microcode_RTD,
		word_sized,
		not_before_68010 | uses_stack,
	};
	
	instruction decoded_RTS =
	{
		fetches_none,
		microcode_RTS,
		unsized,
		uses_stack,
	};
	
	instruction decoded_TRAPV =
	{
		fetches_none,
		microcode_TRAPV
	};
	
	instruction decoded_RTR =
	{
		fetches_none,
		microcode_RTR,
		unsized,
		uses_stack,
	};
	
	instruction decoded_MOVEC =
	{
		fetches_MOVEC,
		microcode_MOVEC,
		long_sized,
		privileged | not_before_68010
	};
	
	instruction decoded_MOVEQ =
	{
		fetches_MOVEQ,
		microcode_MOVE,
		long_sized,
		stores_data | basic_CCR_update
	};
	
	instruction decoded_EXG =
	{
		fetches_EXG,
		microcode_EXG,
		long_sized
	};
	
}
