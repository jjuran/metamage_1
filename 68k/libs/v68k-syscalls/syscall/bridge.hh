/*
	bridge.hh
	---------
*/

#ifndef SYSCALL_BRIDGE_HH
#define SYSCALL_BRIDGE_HH

// v68k
#include "v68k/state.hh"


namespace v68k    {
namespace syscall {

extern uint32_t errno_ptr_addr;

extern int32_t fake_pid;

int32_t bridge( v68k::processor_state& s );

}  // namespace syscall
}  // namespace v68k

#endif
