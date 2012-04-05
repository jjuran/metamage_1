/*
	bridge.hh
	---------
*/

#ifndef CALLBACK_BRIDGE_HH
#define CALLBACK_BRIDGE_HH

// v68k
#include "v68k/emulator.hh"


namespace v68k     {
namespace callback {

enum
{
	unimplemented,
	illegal_instruction,
	division_by_zero,
	chk_trap,
	trapv_trap,
	privilege_violation,
	trace_exception,
	line_F_emulator,
	unimplemented_trap,
	NewPtr_trap,
	DisposePtr_trap,
	ExitToShell_trap,
	SysBeep_trap,
	no_op,
	n
};

inline uint32_t callback_address( int index )
{
	return uint32_t( (index + 1) * -2 );
}

uint32_t bridge( v68k::emulator& emu );

}  // namespace v68k
}  // namespace callback


#endif

