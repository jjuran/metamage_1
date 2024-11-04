/*
	load.hh
	-------
*/

#ifndef EXEC_LOAD_HH
#define EXEC_LOAD_HH

// Standard C
#include <signal.h>
#include <stdint.h>

// v68k
#include "v68k/emulator.hh"


namespace v68k {
namespace exec {

enum
{
	code_address = 30720,
	
	params_addr = 8192,
	
	argc_addr = params_addr + 40,  // 4 bytes
	argv_addr = params_addr + 44,  // 4 bytes
	args_addr = params_addr + 48,
};

struct module_spec
{
	const char*  name;
	
	char* const*  begin;
	char* const*  end;
};

extern module_spec* module_specs;

extern bool tracing;

void load_and_run( uint8_t* mem, emulator& emu, int argc, char* const* argv );

}  // namespace exec
}  // namespace v68k

#endif
