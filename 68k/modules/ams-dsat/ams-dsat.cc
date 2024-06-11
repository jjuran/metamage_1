/*
	ams-dsat.cc
	-----------
*/

// POSIX
#include <unistd.h>

// ams-common
#include "callouts.hh"

// ams-dsat
#include "SysError.hh"


#define PROGRAM  "ams-dsat"

#define STR_LEN( s )  "" s, (sizeof s - 1)

#define WARN( msg )  write( STDERR_FILENO, STR_LEN( PROGRAM ": " msg "\n" ) )


#ifndef __TRAPS__

enum
{
	_Unimplemented = 0xA89F,
	_SysError      = 0xA9C9,
};

#endif

void* os_trap_table     [] : 1 * 1024;
void* toolbox_trap_table[] : 3 * 1024;

#define TBTRAP( Routine )  (toolbox_trap_table[ _ ## Routine & 0x03FF ] = &Routine ## _patch)

static
asm void unimplemented_trap()
{
	MOVEQ    #12,D0
	
	_SysError | 0x0400  // auto-pop
}

static
void reassign_unimplemented_traps()
{
	void* old_addr = toolbox_trap_table[ _Unimplemented & 0x03FF ];
	void* new_addr = &unimplemented_trap;
	
	short n;
	void** p;
	
	n = 256;
	p = os_trap_table;
	
	while ( --n >= 0 )
	{
		if ( *p++ == old_addr )
		{
			p[ -1 ] = new_addr;
		}
	}
	
	n = 1024;
	p = toolbox_trap_table;
	
	while ( --n >= 0 )
	{
		if ( *p++ == old_addr )
		{
			p[ -1 ] = new_addr;
		}
	}
}

int main( int argc, char** argv )
{
	if ( argc > 1 )
	{
		WARN( "no arguments allowed" );
		
		_exit( 1 );
	}
	
	TBTRAP( SysError );  // A9C9
	
	reassign_unimplemented_traps();
	
	module_suspend();  // doesn't return
}
