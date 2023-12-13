/*
	serial-ports.cc
	---------------
*/

// command
#include "command/get_option.hh"

// mac-sys-utils
#include "mac_sys/gestalt.hh"

// serial-ports
#include "CTB.hh"
#include "IOKit.hh"


#ifndef TARGET_API_MAC_CARBON
#define TARGET_API_MAC_CARBON  0
#endif

#ifndef TARGET_RT_MAC_MACHO
#ifdef __MACH__
#define TARGET_RT_MAC_MACHO  1
#else
#define TARGET_RT_MAC_MACHO  0
#endif
#endif


enum
{
	gestaltCTBVersion = 'ctbv',
};

enum
{
	Opt_all     = 'a',
	Opt_verbose = 'v',
};

static command::option options[] =
{
	{ "all",     Opt_all     },
	{ "verbose", Opt_verbose },
	
	{}
};

static bool all_types;

static int verbosity;

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
			
			case Opt_verbose:
				++verbosity;
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
	
	if ( TARGET_RT_MAC_MACHO )
	{
		list_IOKit_serial_ports( all_types );
	}
	else if ( TARGET_API_MAC_CARBON )
	{
		// do nothing
	}
	else if ( mac::sys::gestalt( gestaltCTBVersion ) )
	{
		list_CTB_serial_ports( verbosity );
	}
	
	return 0;
}
