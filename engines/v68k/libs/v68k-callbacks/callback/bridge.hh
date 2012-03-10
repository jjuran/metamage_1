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
	privilege_violation,
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

