/*
	diagnostics.hh
	--------------
*/

#ifndef DIAGNOSTICS_HH
#define DIAGNOSTICS_HH

// Standard C
#include <stdint.h>


void err_allocation_failed( uint32_t n_bytes );

void err_argv_too_big();

void err_argv_way_too_big();

void note_program_loaded( uint32_t n_bytes, const char* path );

void print_blank_line();

void print_instruction_limit_exceeded( const char* limit );

void print_halted();

void print_stopped();

void print_breakpoint( int number );

void print_register_dump( const uint32_t  d[],
                          const uint32_t  a[],
                          uint32_t        alt_sp,
                          uint32_t        alt_ssp,
                          uint32_t        pc,
                          uint16_t        sr );

#endif

