/*
	ams-pack.cc
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

// ams-pack
#include "Packages.hh"


#define STR_LEN( s )  "" s, (sizeof s - 1)

#define PROGRAM  "ams-pack"

#define WARN( msg )  write( STDERR_FILENO, STR_LEN( PROGRAM ": " msg "\n" ) )


void* toolbox_trap_table[] : 3 * 1024;

#define TBTRAP( Proc )  (toolbox_trap_table[ _##Proc & 0x03FF ] = &Proc##_patch)


static void install_Packages()
{
	TBTRAP( InitPack     );  // A9E5
	TBTRAP( InitAllPacks );  // A9E6
	
	TBTRAP( Pack0 );  // A9E7
	TBTRAP( Pack1 );  // A9E8
	TBTRAP( Pack2 );  // A9E9
	TBTRAP( Pack3 );  // A9EA
	TBTRAP( Pack4 );  // A9EB
	TBTRAP( Pack5 );  // A9EC
	TBTRAP( Pack6 );  // A9ED
	TBTRAP( Pack7 );  // A9EE
}

int main( int argc, char** argv )
{
	if ( argc > 1 )
	{
		WARN( "no arguments allowed" );
		
		_exit( 1 );
	}
	
	install_Packages();
	
	module_suspend();  // doesn't return
}
