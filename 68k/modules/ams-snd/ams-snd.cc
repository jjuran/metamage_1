/*
	ams-snd.cc
	----------
*/

// Mac OS
#ifndef __TRAPS__
#include <Traps.h>
#endif

// POSIX
#include <unistd.h>

// ams-common
#include "callouts.hh"

// ams-snd
#include "Sound-1_0.hh"
#include "SoundDispatch.hh"


#define STR_LEN( s )  "" s, (sizeof s - 1)

#define PROGRAM  "ams-snd"

#define WARN( msg )  write( STDERR_FILENO, STR_LEN( PROGRAM ": " msg "\n" ) )


void* toolbox_trap_table[] : 3 * 1024;

#define TBTRAP( Proc )  (toolbox_trap_table[ _##Proc & 0x03FF ] = &Proc##_patch)


static
void install_SoundManager()
{
	TBTRAP( SoundDispatch     );  // A800
	TBTRAP( SndDisposeChannel );  // A801
	
	TBTRAP( SndPlay );  // A805
	
	TBTRAP( SndNewChannel );  // A807
}

int main( int argc, char** argv )
{
	if ( argc > 1 )
	{
		WARN( "no arguments allowed" );
		
		_exit( 1 );
	}
	
	install_SoundManager();
	
	module_suspend();  // doesn't return
}
