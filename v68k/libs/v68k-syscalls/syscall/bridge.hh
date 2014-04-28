/*
	bridge.hh
	---------
*/

#ifndef SYSCALLBRIDGE_HH
#define SYSCALLBRIDGE_HH

// v68k
#include "v68k/state.hh"


extern uint32_t errno_ptr_addr;

extern int32_t fake_pid;

bool bridge_call( v68k::processor_state& s );


#endif

