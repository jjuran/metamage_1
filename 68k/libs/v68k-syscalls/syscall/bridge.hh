/*
	bridge.hh
	---------
*/

#ifndef SYSCALL_BRIDGE_HH
#define SYSCALL_BRIDGE_HH

// v68k
#include "v68k/state.hh"


extern uint32_t errno_ptr_addr;

extern int32_t fake_pid;

v68k::op_result bridge_call( v68k::processor_state& s );


#endif
