/*
	ams-fs.cc
	---------
*/

// Mac OS
#ifndef __TRAPS__
#include <Traps.h>
#endif

// POSIX
#include <unistd.h>

// ams-fs
#include "Files.hh"


#define STR_LEN( s )  "" s, (sizeof s - 1)

#define PROGRAM  "ams-fs"

#define WARN( msg )  write( STDERR_FILENO, STR_LEN( PROGRAM ": " msg "\n" ) )


void* os_trap_table[] : 1 * 1024;

#define OSTRAP( Proc )  (os_trap_table[ _##Proc & 0x00FF ] = &Proc##_patch)


static
void install_FileManager()
{
	old_Open = (Open_ProcPtr) os_trap_table[ _Open & 0x00FF ];
	
	OSTRAP( Open   );  // A000
	
	OSTRAP( Create );  // A008
	
	OSTRAP( FlushVol );  // A013
	OSTRAP( GetVol   );  // A014
}

static
asm void module_suspend()
{
	JSR      0xFFFFFFF8
}

int main( int argc, char** argv )
{
	if ( argc > 0 )
	{
		char* const* args = ++argv;
		
		if ( *args != NULL )
		{
			WARN( "no arguments allowed" );
			
			_exit( 1 );
		}
	}
	
	install_FileManager();
	
	module_suspend();  // doesn't return
}
