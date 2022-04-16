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
	
	enum instruction_flags_t
	{
		privileged_except_on_68000 = 0x01,
		privileged                 = 0x02,
		privilege_mask             = 0x03,
		
		not_before_68010 = 0x10,
		not_before_68020 = 0x20,
		not_before_68030 = 0x30,
		not_before_68040 = 0x40,
		not_before_mask  = 0x70,
		
		loads_and        = 0x1000,
		stores_data      = 0x2000,
		uses_stack       = 0x4000,
		byte_access      = 0x8000,  // all access is byte-sized:  MOVEP
		
		CCR_update_set_X = 0x0080,  // Assign C to X
		CCR_update_add   = 0x0000,
		CCR_update_sub   = 0x0100,
		CCR_update_and_Z = 0x0200,  // Z &= x == 0
		CCR_update_div_q = 0x0200,  // Consider only quotient, not remainder
		CCR_update_An    = 0x0800,  // Update CCR even for address registers
		
		no_CCR_update    = 0x0000,
		basic_CCR_update = 0x0400,  // N = x < 0, Z = x == 0, V = 0, C = 0
		BTST_CCR_update  = 0x0500,
		ADD_CCR_update   = CCR_update_add | CCR_update_set_X,  // 0x0080, 0
		SUB_CCR_update   = CCR_update_sub | CCR_update_set_X,  // 0x0180, 1
		CMP_CCR_update   = CCR_update_sub | CCR_update_An,     // 0x0900, 1
		ADDX_CCR_update  = ADD_CCR_update | CCR_update_and_Z,  // 0x0280, 2
		SUBX_CCR_update  = SUB_CCR_update | CCR_update_and_Z,  // 0x0380, 3
		
		TST_CCR_update   = basic_CCR_update | CCR_update_An,   // 0x0C00, 4
		DIV_CCR_update   = basic_CCR_update | CCR_update_div_q,// 0x0600, 6
		
		/*
			Omit flags from the mask, so that the shifted mask result can be
			used as an array index, except include the set-X flag (which will
			be shifted off anyway) so that the null case can be distinguished
			from ADD.
		*/
		
		CCR_update_mask  = 0x0780
	};
	
	enum
	{
		CCR_update_shift = 8
	};
	
	inline instruction_flags_t operator|( instruction_flags_t a, instruction_flags_t b )
	{
		return instruction_flags_t( int( a ) | int( b ) );
	}
	
	inline instruction_flags_t& operator|=( instruction_flags_t& a, instruction_flags_t b )
	{
		return a = a | b;
	}
	
	struct instruction
	{
		typedef instruction_flags_t flags_t;
		
		fetcher*   fetch;
		microcode  code;
		op_size_t  size;
		flags_t    flags;
		
		bool accesses_bytes_only() const  { return flags & byte_access; }
	};
	
}

#endif
