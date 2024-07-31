/*
	native.cc
	---------
*/

#include "native/native.hh"

// Standard C
#include <string.h>

// log-of-war
#include "logofwar/report.hh"

// v68k
#include "v68k/print.hh"


#ifdef __MWERKS__
#pragma exceptions off
#endif


namespace v68k   {
namespace native {

const uint32_t os_trap_table_address = 1024;
const uint32_t tb_trap_table_address = 3072;


static bool spinloop_flag;

static inline
bool is_spinloop_memtest( uint16_t opcode )
{
	switch ( opcode )
	{
		case 0x4A38:  // TST.B    0xXXXX
		case 0x4A2D:  // TST.B    (offset,A5)
		case 0x4A6D:  // TST.W    (offset,A5)
			return true;
		
		default:
			return false;
	}
}

static inline
bool is_spinloop_branch( uint16_t opcode )
{
	return opcode == 0x66FA;  // BNE.S    *-4
}

/*
	The Metamage codebase includes a 68K asm implementation of mempcpy(),
	an extension to the standard C library.  It's a useful alternative to
	memcpy() for several reasons.  First, it returns the incremented dst
	pointer, which is less work than saving and restoring the unmodified
	value, and the caller may well want the incremented result anyway.
	Second, it doesn't waste time checking the length before entering the
	byte copy loop for small lengths (as MSL's memcpy() does).  Third, it
	never incurs trap dispatch overhead to call BlockMoveData().  Finally,
	as an extern function, it avoids codegen bloat.
	
	The downside is that we miss out on any optimizations for large copies
	implemented in BlockMoveData() itself, if any.  In general, I suggest
	using purpose-matched variants of memcpy() when performance warrants it,
	e.g. one that expects word-aligned data or only copies up to 32K, etc.
	That way, those variants can be optimized, and the general case doesn't
	have to check at run time, every time, to see if such optimizations can
	be applied.
	
	That said, when code runs in xv68k, we would very much like to bypass
	the byte copy loop and execute the copy natively.  MSL's memcpy(), with
	its provision for calling BlockMoveData(), actually did this -- how sad
	that our "optimization" of using mempcpy() defeats that!  AMS modules
	already know to call fast_mempcpy() instead, which calls out to a native
	implementation, but sometimes they share libraries with non-AMS programs,
	so an xv68k-dependent approach isn't workable in that case.  Our solution
	is to recognize mempcpy() in emulated code as it's executed, and patch
	it to call fast_mempcpy() instead.
	
	This optimization is especially important to ams-fs's performance as a
	Freemount client, since the plus::string objects used as buffers call
	mempcpy() internally.  Region intersection calculations in ams-qd also
	make calls to mempcpy(), affecting performance with complex regions.
	
	In general, this approach could run afoul of a few applications which
	checksum their code, but we'll deal with that when it actually happens.
	
	mempcpy:
	
		TST.L    D0                 // 4a80
		BEQ.S    *+8 (done)         // 6706
	loop:
		MOVE.B   (A1)+,(A0)+        // 10d9
		SUBQ.L   #1,D0              // 5380
		BNE.S    *-4 (loop)         // 66fa
	done:
		RTS                         // 4e75
	
	mempcpy_patched:
	
		TST.L    D0                 // 4a80
		BEQ.S    *+8 (done)         // 6706
	loop:
		JMP      fast_mempcpy       // 4ef8ffd0
	not_reached:
		BNE.S    *-4 (loop)         // 66fa
	done:
		RTS                         // 4e75
	
*/

#define MEMPCPY  \
	"\x4a\x80" \
	"\x67\x06" \
	"\x10\xd9" \
	"\x53\x80" \
	"\x66\xfa" \
	"\x4e\x75"

#define JMP_FAST_MEMPCPY  "\x4e\xf8\xff\xd0"

static
bool patched_mempcpy( v68k::emulator& emu )
{
	using v68k::PC;
	using v68k::mem_write;
	
	if ( emu.opcode == 0x10D9 )
	{
		uint32_t pc = emu.regs[ PC ];
		
		if ( uint8_t* p = emu.translate( pc, 8, emu.data_space(), mem_write ) )
		{
			if ( memcmp( p, &MEMPCPY[ 4 ], 8 ) == 0 )
			{
				memcpy( p, JMP_FAST_MEMPCPY, 4 );
				
				emu.prefetch_instruction_word();
				return true;
			}
		}
	}
	
	return false;
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
	using v68k::D0;
	using v68k::D1;
	using v68k::D2;
	using v68k::A0;
	using v68k::A1;
	using v68k::SP;
	using v68k::PC;
	using v68k::function_code_t;
	
	if ( patched_mempcpy( emu ) )
	{
		/*
			We've patched the instruction, but we didn't execute it here.
		*/
		
		return false;
	}
	
	if ( spinloop_flag  &&  is_spinloop_memtest( emu.opcode ) )
	{
		const int level  = 1;
		const int vector = 88;
		
		WARNING = "Patching spinloop at ", (v68k::hex32_t) emu.regs[ PC ];
		
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
		}
		
		emu.prefetch_instruction_word();
		
		return true;
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
		
		const int16_t d0 = emu.regs[ D0 ];
		
		emu.sr.nzvc = (d0 < 0) << 3 | (d0 == 0) << 2;
		
		emu.get_long( emu.regs[ SP ], emu.regs[ PC ], data_space );
		
		emu.regs[ SP ] += 4;  // pop saved return address
		
		emu.prefetch_instruction_word();
		
		return true;
	}
	
	return false;
}

}  // namespace native
}  // namespace v68k
