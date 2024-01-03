/*
	sndtrack.cc
	-----------
*/

// POSIX
#include <unistd.h>

// Standard C
#include <signal.h>

// command
#include "command/get_option.hh"

// sndtrack
#include "event_loop.hh"
#include "portaudio.hh"
#include "recording.hh"


enum
{
	Option_record = 'o',
};

static command::option options[] =
{
	{ "rec",    Option_record, command::Param_required },
	{ "record", Option_record, command::Param_required },
	
	{ NULL }
};

static
char* const* get_options( char** argv )
{
	int opt;
	
	++argv;  // skip arg 0
	
	while ( (opt = command::get_option( (char* const**) &argv, options )) > 0 )
	{
		using command::global_result;
		
		switch ( opt )
		{
			case Option_record:
				start_recording( global_result.param );
				break;
			
			default:
				break;
		}
	}
	
	return argv;
}

int main( int argc, char* argv[] )
{
	if ( argc == 0 )
	{
		return 0;  // whatever
	}
	
	char* const* args = get_options( argv );
	
	int argn = argc - (args - argv);
	
	if ( signal( SIGINT, SIG_IGN ) != SIG_IGN )
	{
		signal( SIGINT, &interrupt );
	}
	
	event_loop( STDIN_FILENO, &portaudio::start, &portaudio::stop );
	
	end_recording();
	
	return 0;
}
