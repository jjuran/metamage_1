/*
	serial-ports.cc
	---------------
*/

// command
#include "command/get_option.hh"

// serial-ports
#include "IOKit.hh"



enum
{
	Opt_all = 'a',
};

static command::option options[] =
{
	{ "all", Opt_all },
	
	{}
};

static bool all_types;

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
			case Opt_all:
				all_types = true;
				break;
			
			default:
				break;
		}
	}
	
	return argv;
}

int main( int argc, char** argv )
{
	char* const* args = get_options( argv );
	
	int argn = argc - (args - argv);
	
	list_IOKit_serial_ports( all_types );
	
	return 0;
}
