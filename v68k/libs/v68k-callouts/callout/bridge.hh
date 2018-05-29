/*
	bridge.hh
	---------
*/

#ifndef CALLOUT_BRIDGE_HH
#define CALLOUT_BRIDGE_HH

// v68k
#include "v68k/state.hh"


namespace v68k    {
namespace callout {

enum
{
	no_op,
	load,
	enter_supervisor_mode,
	module_suspend,
	
	trace_reserved,
	trace_all,
	trace_flow,
	trace_off,
	
	ScrnBitMap_trap,
	lock_screen,
	unlock_screen,
	reserved1,
	
	alloc,
	dealloc,
	
	system_call,
	
	bus_error,
	address_error,
	illegal_instruction,
	division_by_zero,
	chk_trap,
	trapv_trap,
	privilege_violation,
	trace_exception,
	line_A_emulator,
	line_F_emulator,
	format_error,
	
	unimplemented_trap,
	BlockMove_trap,
	Gestalt_trap,
	unimplemented,
	n
};

inline uint32_t callout_address( int index )
{
	return uint32_t( (index + 1) * -2 );
}

uint32_t bridge( v68k::processor_state& emu );

}  // namespace callout
}  // namespace v68k


#endif
