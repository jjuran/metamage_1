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
	
	instruction decoded_LINK_L =
	{
		fetches_LINK_L,
		microcode_LINK,  // same as LINK
		not_before_68020
	};
	
	instruction decoded_BKPT =
	{
		fetches_data_at_0007,
		microcode_BKPT
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
		fetches_STOP,
		microcode_STOP,
		privileged
	};
	
	instruction decoded_RTS =
	{
		fetches_none,
		microcode_RTS
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
	
	instruction decoded_MOVEQ =
	{
		fetches_MOVEQ,
		microcode_MOVEQ
	};
	
	instruction decoded_EXG =
	{
		fetches_EXG,
		microcode_EXG
	};
	
}

