/*
	bridge.hh
	---------
*/

#ifndef SYSCALLBRIDGE_HH
#define SYSCALLBRIDGE_HH

// v68k
#include "v68k/emulator.hh"


extern uint32_t errno_ptr_addr;

bool bridge_call( v68k::emulator& emu );


#endif

