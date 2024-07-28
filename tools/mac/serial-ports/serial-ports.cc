/*
	serial-ports.cc
	---------------
*/

// command
#include "command/get_option.hh"

// mac-config
#include "mac_config/open-transport.hh"

// mac-sys-utils
#include "mac_sys/gestalt.hh"

// serial-ports
#include "classic.hh"
#include "CTB.hh"
#include "IOKit.hh"
#include "OT.hh"


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

/*
	Open Transport is available in pre-Carbon (and the headers
	are always present for all builds via Universal Interfaces),
	but linking is an issue.  For PPC, we may want to split up
	the OpenTransportApp pseudoproject and may need to update the
	symbol redefinition warning filter for `errno`.  For 68K, we
	may need to create wrappers that preserve A4 around API calls.
*/

#ifndef CONFIG_OT
#define CONFIG_OT (CONFIG_OPEN_TRANSPORT_HEADERS  &&  TARGET_API_MAC_CARBON)
#endif


#ifndef __GESTALT__

enum
{
	gestaltCTBVersion = 'ctbv',
};

#endif

enum
{
	Opt_all     = 'a',
	Opt_verbose = 'v',
	
	Opt_last_byte = 255,
	
	Opt_OT,
};

static command::option options[] =
{
#if CONFIG_OT
	{ "OT",      Opt_OT      },
#endif
	{ "all",     Opt_all     },
#if CONFIG_OT
	{ "ot",      Opt_OT      },
#endif
	{ "verbose", Opt_verbose },
	
	{}
};

static bool all_types;
static bool use_OT;

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
			
		#if CONFIG_OT
			
			case Opt_OT:
				use_OT = true;
				break;
			
		#endif
			
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
	
	if ( TARGET_RT_MAC_MACHO  &&  ! use_OT )
	{
		list_IOKit_serial_ports( all_types );
	}
	else if ( TARGET_API_MAC_CARBON )
	{
		list_OT_serial_ports( verbosity );
	}
	else if ( mac::sys::gestalt( gestaltCTBVersion ) )
	{
		list_CTB_serial_ports( verbosity );
	}
	else
	{
		list_classic_serial_ports( verbosity );
	}
	
	return 0;
}
