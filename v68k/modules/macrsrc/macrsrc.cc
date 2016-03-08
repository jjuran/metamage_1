/*
	macrsrc.cc
	----------
*/

// Mac OS
#ifndef __TRAPS__
#include <Traps.h>
#endif

// POSIX
#include <unistd.h>

// macrsrc
#include "Resources.hh"


#define STR_LEN( s )  "" s, (sizeof s - 1)


void* os_trap_table     [] : 1 * 1024;
void* toolbox_trap_table[] : 3 * 1024;

#define OSTRAP( Proc )  (os_trap_table     [ _##Proc & 0x00FF ] = &Proc##_patch)
#define TBTRAP( Proc )  (toolbox_trap_table[ _##Proc & 0x03FF ] = &Proc##_patch)


static
void install_ResourceManager()
{
	TBTRAP( GetResource     );  // A9A0
	TBTRAP( ReleaseResource );  // A9A3
}

static
asm void module_suspend()
{
	MOVE.L   A4,module_A4
	JSR      0xFFFFFFF8
}

int main( int argc, char** argv )
{
	if ( argc > 1 )
	{
		write( STDERR_FILENO, STR_LEN( "macrsrc: no arguments allowed\n" ) );
		
		_exit( 1 );
	}
	
	install_ResourceManager();
	
	module_suspend();  // doesn't return
}
