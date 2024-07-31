/*
	native.hh
	---------
*/

#ifndef NATIVE_NATIVE_HH
#define NATIVE_NATIVE_HH

// v68k
#include "v68k/emulator.hh"


namespace v68k   {
namespace native {

bool native_override( v68k::emulator& emu );

}  // namespace native
}  // namespace v68k

#endif
