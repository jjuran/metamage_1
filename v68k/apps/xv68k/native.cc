/*
	native.cc
	---------
*/

#include "native.hh"


#pragma exceptions off


const uint32_t tb_trap_table_address = 3072;


static
uint32_t get_toolbox_trap_addr( v68k::emulator& emu, uint16_t trap_word )
{
	const uint32_t trap_table_addr = tb_trap_table_address;
	const uint32_t trap_entry_addr = trap_table_addr + 4 * (trap_word & 0x3FF);
	
	uint32_t trap_addr;
	emu.mem.get_long( trap_entry_addr, trap_addr, emu.data_space() );
	
	return trap_addr;
}

bool native_override( v68k::emulator& emu )
{
	using v68k::D1;
	using v68k::SP;
	using v68k::PC;
	using v68k::function_code_t;
	
	if ( (emu.opcode & 0xF000) == 0xA000 )
	{
		const uint16_t trap_word = emu.opcode;
		
		const function_code_t data_space = emu.data_space();
		
		if ( const bool toolbox = trap_word >= 0xA800 )
		{
			const uint16_t unimplemented_trap = 0xA89F;
			
			const uint32_t trap_addr = get_toolbox_trap_addr( emu, trap_word );
			
			if ( const bool no_autopop = trap_word < 0xAC00 )
			{
				const uint32_t stack_top = emu.regs[ SP ] -= 4;
				
				emu.mem.put_long( stack_top, emu.regs[ PC ] + 2, data_space );
			}
			
			emu.regs[ PC ] = trap_addr;
			
			emu.prefetch_instruction_word();
			
			return true;
		}
	}
	
	return false;
}
