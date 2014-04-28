/*
	bridge.hh
	---------
*/

#ifndef CALLBACK_BRIDGE_HH
#define CALLBACK_BRIDGE_HH

// v68k
#include "v68k/state.hh"


namespace v68k     {
namespace callback {

enum
{
	no_op,
	load,
	enter_supervisor_mode,
	reserved_3,
	trace_reserved,
	trace_all,
	trace_flow,
	trace_off,
	illegal_instruction,
	division_by_zero,
	chk_trap,
	trapv_trap,
	privilege_violation,
	trace_exception,
	line_A_emulator,
	line_F_emulator,
	unimplemented_trap,
	NewPtr_trap,
	DisposePtr_trap,
	BlockMove_trap,
	Gestalt_trap,
	unimplemented,
	n
};

inline uint32_t callback_address( int index )
{
	return uint32_t( (index + 1) * -2 );
}

uint32_t bridge( v68k::processor_state& emu );

}  // namespace v68k
}  // namespace callback


#endif

