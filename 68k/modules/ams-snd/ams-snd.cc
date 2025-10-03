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

// command
#include "command/get_option.hh"

// gear
#include "gear/parse_decimal.hh"

// ams-common
#include "callouts.hh"

// ams-snd
#include "Sound-1_0.hh"
#include "SoundDispatch.hh"


#define STR_LEN( s )  "" s, (sizeof s - 1)

#define PROGRAM  "ams-snd"

#define WARN( msg )  write( STDERR_FILENO, STR_LEN( PROGRAM ": " msg "\n" ) )


enum
{
	Opt_max_channels = 'n',
};

static command::option options[] =
{
	{ "max-channels", Opt_max_channels, command::Param_required },
	
	NULL,
};


void* toolbox_trap_table[] : 3 * 1024;

#define TBTRAP( Proc )  (toolbox_trap_table[ _##Proc & 0x03FF ] = &Proc##_patch)


static
void install_SoundManager()
{
	TBTRAP( SoundDispatch     );  // A800
	TBTRAP( SndDisposeChannel );  // A801
	
	TBTRAP( SndDoCommand      );  // A803
	TBTRAP( SndDoImmediate    );  // A804
	TBTRAP( SndPlay           );  // A805
	
	TBTRAP( SndNewChannel );  // A807
}

static
char* const* get_options( char** argv )
{
	using command::global_result;
	
	using gear::parse_unsigned_decimal;
	
	int opt;
	
	++argv;  // skip arg 0
	
	while ( (opt = command::get_option( (char* const**) &argv, options )) > 0 )
	{
		switch ( opt )
		{
			case Opt_max_channels:
				max_channels = parse_unsigned_decimal( global_result.param );
				break;
			
			default:
				break;
		}
	}
	
	return argv;
}

int main( int argc, char** argv )
{
	if ( argc > 0 )
	{
		char* const* args = get_options( argv );
		
		if ( *args != NULL )
		{
			WARN( "no arguments allowed" );
			
			_exit( 1 );
		}
	}
	
	install_SoundManager();
	
	module_suspend();  // doesn't return
}
