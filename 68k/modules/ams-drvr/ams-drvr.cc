/*
	ams-drvr.cc
	-----------
*/

// Mac OS
#ifndef __TRAPS__
#include <Traps.h>
#endif

// POSIX
#include <unistd.h>

// ams-common
#include "callouts.hh"

// ams-drvr
#include "Drivers.hh"


#define STR_LEN( s )  "" s, (sizeof s - 1)

#define PROGRAM  "ams-drvr"

#define WARN( msg )  write( STDERR_FILENO, STR_LEN( PROGRAM ": " msg "\n" ) )


void* os_trap_table[] : 1 * 1024;

#define OSTRAP( Proc )  (os_trap_table[ _##Proc & 0x00FF ] = &Proc##_patch)


static
void patch_DeviceManager()
{
	old_Open  = (Open_ProcPtr) os_trap_table[ _Open  & 0x00FF ];
	
	OSTRAP( Open );  // A000
}

int main( int argc, char** argv )
{
	if ( argc > 0 )
	{
		char* const* args = argv + 1;
		
		if ( *args != NULL )
		{
			WARN( "no arguments allowed" );
			
			_exit( 1 );
		}
	}
	
	patch_DeviceManager();
	
	module_suspend();  // doesn't return
}
