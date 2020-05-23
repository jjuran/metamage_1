/*
	native.cc
	---------
*/

#include "native.hh"


#ifdef __MWERKS__
#pragma exceptions off
#endif


const uint32_t os_trap_table_address = 1024;
const uint32_t tb_trap_table_address = 3072;


static bool spinloop_flag;

static inline
bool is_spinloop_memtest( uint16_t opcode )
{
	return opcode == 0x4A6D;  // TST.W    (offset,A5)
}

static inline
bool is_spinloop_branch( uint16_t opcode )
{
	return opcode == 0x66FA;  // BNE.S    *-4
}

static
uint32_t get_toolbox_trap_addr( v68k::emulator& emu, uint16_t trap_word )
{
	const uint32_t trap_table_addr = tb_trap_table_address;
	const uint32_t trap_entry_addr = trap_table_addr + 4 * (trap_word & 0x3FF);
	
	uint32_t trap_addr;
	emu.get_long( trap_entry_addr, trap_addr, emu.data_space() );
	
	return trap_addr;
}

bool native_override( v68k::emulator& emu )
{
	using v68k::D1;
	using v68k::D2;
	using v68k::A0;
	using v68k::A1;
	using v68k::SP;
	using v68k::PC;
	using v68k::function_code_t;
	
	if ( spinloop_flag  &&  is_spinloop_memtest( emu.opcode ) )
	{
		const int level  = 1;
		const int vector = 88;
		
		emu.interrupt( level, vector );
		
		return false;
	}
	
	spinloop_flag = is_spinloop_branch( emu.opcode );
	
	if ( (emu.opcode & 0xF000) == 0xA000 )
	{
		const uint16_t trap_word = emu.opcode;
		
		const function_code_t data_space = emu.data_space();
		
		if ( const bool toolbox = trap_word >= 0xA800 )
		{
			const uint16_t unimplemented_trap = 0xA89F;
			
			const uint32_t trap_addr = get_toolbox_trap_addr( emu, trap_word );
			
			if ( trap_addr == get_toolbox_trap_addr( emu, unimplemented_trap ) )
			{
				emu.regs[ D1 ] = trap_word;
			}
			
			if ( const bool no_autopop = trap_word < 0xAC00 )
			{
				const uint32_t stack_top = emu.regs[ SP ] -= 4;
				
				emu.put_long( stack_top, emu.regs[ PC ] + 2, data_space );
			}
			
			emu.regs[ PC ] = trap_addr;
			
			emu.prefetch_instruction_word();
			
			return true;
		}
		else
		{
			// OS trap
			
			const uint16_t trap_num = trap_word & 0xFF;
			
			uint32_t pointer_arg = 0;
			
			if ( trap_num == 0x00F6 )
			{
				emu.get_long( emu.regs[ SP ], pointer_arg, data_space );
				
				emu.regs[ SP ] += 4;
			}
			
			const uint32_t trap_table_addr = os_trap_table_address;
			const uint32_t trap_entry_addr = trap_table_addr + 4 * trap_num;
			
			uint32_t trap_addr;
			emu.get_long( trap_entry_addr, trap_addr, data_space );
			
			emu.put_long( emu.regs[ SP ] -= 4, emu.regs[ PC ] + 2, data_space );
			
			emu.put_long( emu.regs[ SP ] -= 4, emu.regs[ A1 ], data_space );
			emu.put_long( emu.regs[ SP ] -= 4, emu.regs[ A0 ], data_space );
			emu.put_long( emu.regs[ SP ] -= 4, emu.regs[ D2 ], data_space );
			emu.put_long( emu.regs[ SP ] -= 4, emu.regs[ D1 ], data_space );
			
			emu.regs[ D1 ] = trap_word;
			
			if ( trap_num == 0x00F6 )
			{
				emu.regs[ A0 ] = pointer_arg;
			}
			
			emu.put_word( emu.regs[ SP ] -= 2, trap_word, data_space );
			emu.put_word( emu.regs[ SP ] -= 2, 0x484F,    data_space );
			
			// Push the stack pointer as the return address
			
			const uint32_t sp = emu.regs[ SP ];
			
			emu.put_long( emu.regs[ SP ] -= 4, sp, data_space );
			
			emu.regs[ PC ] = trap_addr;
			
			emu.prefetch_instruction_word();
			
			return true;
		}
	}
	
	if ( emu.opcode == 0x484F )
	{
		// BKPT #7
		
		const function_code_t data_space = emu.data_space();
		
		emu.regs[ SP ] += 2;  // pop BKPT instruction word
		
		uint16_t trap_word;
		emu.get_word( emu.regs[ SP ], trap_word, data_space );
		
		emu.regs[ SP ] += 2;  // pop trap word
		
		emu.get_long( emu.regs[ SP ], emu.regs[ D1 ], data_space );
		
		emu.regs[ SP ] += 4;  // pop saved D1
		
		emu.get_long( emu.regs[ SP ], emu.regs[ D2 ], data_space );
		
		emu.regs[ SP ] += 4;  // pop saved D2
		
		if ( const bool keep_A0 = trap_word & 0x100 )
		{
			// don't restore A0
		}
		else
		{
			emu.get_long( emu.regs[ SP ], emu.regs[ A0 ], data_space );
		}
		
		emu.regs[ SP ] += 4;  // pop saved A0
		
		emu.get_long( emu.regs[ SP ], emu.regs[ A1 ], data_space );
		
		emu.regs[ SP ] += 4;  // pop saved A1
		
		emu.get_long( emu.regs[ SP ], emu.regs[ PC ], data_space );
		
		emu.regs[ SP ] += 4;  // pop saved return address
		
		emu.prefetch_instruction_word();
		
		return true;
	}
	
	return false;
}
