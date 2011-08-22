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
		
		loads_and        = 0x0080,
		stores_byte_data = 0x0100,
		stores_word_data = 0x0200,
		stores_long_data = 0x0300,
		stores_data_mask = 0x0300,
		in_register      = 0x0400,
		and_sets_CCR     = 0x0800
	};
	
	inline instruction_flags_t operator|( instruction_flags_t a, instruction_flags_t b )
	{
		return instruction_flags_t( int( a ) | int( b ) );
	}
	
	struct instruction
	{
		typedef instruction_flags_t flags_t;
		
		fetcher*   fetch;
		microcode  code;
		flags_t    flags;
	};
	
}

#endif

