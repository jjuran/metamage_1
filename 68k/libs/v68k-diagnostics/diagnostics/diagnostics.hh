/*
	diagnostics.hh
	--------------
*/

#ifndef DIAGNOSTICS_DIAGNOSTICS_HH
#define DIAGNOSTICS_DIAGNOSTICS_HH

// Standard C
#include <stdint.h>


namespace v68k        {
namespace diagnostics {

void err_allocation_failed( uint32_t n_bytes );

void err_argv_too_big();

void err_argv_way_too_big();

void print_blank_line();

void print_instruction_limit_exceeded( const char* limit );

void print_halted();

void print_stopped();

void print_breakpoint( int number );

}  // namespace diagnostics
}  // namespace v68k

#endif
