/*
	loop.hh
	-------
*/

#ifndef EXEC_LOOP_HH
#define EXEC_LOOP_HH

// Standard C
#include <signal.h>

// v68k
#include "v68k/emulator.hh"


namespace v68k {
namespace exec {

extern bool turbo;
extern bool polling;

extern sig_atomic_t sigint_pending;

extern unsigned long n_instructions;

void emulation_loop( v68k::emulator& emu );

}  // namespace exec
}  // namespace v68k

#endif
