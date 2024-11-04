/*
	loop.cc
	-------
*/

#include "exec/loop.hh"

// log-of-war
#include "logofwar/report.hh"

// v68k-lowmem
#include "lowmem/lowmem.hh"

// v68k-native
#include "native/native.hh"


#ifdef __MWERKS__
#pragma exceptions off
#endif


namespace v68k {
namespace exec {

using logofwar::hex32_t;

bool turbo;
bool polling;

sig_atomic_t sigint_pending;

unsigned long n_instructions;

void emulation_loop( v68k::emulator& emu )
{
	using v68k::native::native_override;
	
	emu.reset();
	
	emu.regs[ 8 + 6 ] = 0;
	
	bool event_poll_interrupt_pending = false;
	
	while ( (turbo  &&  native_override( emu ))  ||  emu.step() )
	{
		n_instructions = emu.instruction_count();
		
	#ifdef __RELIX__
		
		if ( short( n_instructions ) == 0 )
		{
			kill( 1, 0 );  // Guaranteed yield point in MacRelix
		}
		
	#endif
		
		if ( emu.most_recent_PC_during_fault() == 0 )
		{
			uint32_t prev_addr = emu.current_instruction_address();
			
			WARNING = "Execution at $000000 after ", hex32_t( prev_addr );
		}
		
		using v68k::lowmem::ticking;
		
		if ( (short( n_instructions ) == 0  ||  ticking)  &&  polling )
		{
			ticking = false;
			
			event_poll_interrupt_pending = true;
		}
		
		if ( event_poll_interrupt_pending  &&  emu.sr.iii == 0 )
		{
			event_poll_interrupt_pending = false;
			
			const int level  = 1;
			const int vector = 64;
			
			emu.interrupt( level, vector );
		}
		
		if ( sigint_pending )
		{
			sigint_pending = false;
			
			const int signal_number =  2;
			const int signal_vector = 64 + signal_number;
			
			emu.interrupt( 7, signal_vector );
		}
	}
}

}  // namespace exec
}  // namespace v68k
