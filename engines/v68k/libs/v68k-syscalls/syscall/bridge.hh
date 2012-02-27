/*
	bridge.hh
	---------
*/

#ifndef SYSCALLBRIDGE_HH
#define SYSCALLBRIDGE_HH

// v68k
#include "v68k/emulator.hh"


const uint32_t params_addr = 4096;

bool bridge_call( v68k::emulator& emu );


#endif

