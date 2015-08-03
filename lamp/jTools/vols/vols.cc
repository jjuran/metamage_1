/*	=======
 *	vols.cc
 *	=======
 */

// POSIX
#include <unistd.h>

// Standard C
#include <stdlib.h>

// command
#include "command/get_option.hh"

// plus
#include "plus/var_string.hh"

// Nucleus
#include "nucleus/shared.hh"

// poseven
#include "poseven/extras/slurp.hh"
#include "poseven/functions/dirfd.hh"
#include "poseven/functions/open.hh"
#include "poseven/functions/openat.hh"
#include "poseven/functions/opendir.hh"
#include "poseven/functions/write.hh"
#include "poseven/sequences/directory_contents.hh"

// Orion
#include "Orion/Main.hh"


using namespace command::constants;

enum
{
	Option_last_byte = 255,
	
	Option_driver,
	Option_ramdisk,
};

static command::option options[] =
{
	{ "driver", Option_driver  },
	{ "ram",    Option_ramdisk },
	
	{ NULL }
};

static const char* wanted_driver_name = NULL;

static bool ramdisk_only = false;

static char* const* get_options( char* const* argv )
{
	++argv;  // skip arg 0
	
	short opt;
	
	while ( (opt = command::get_option( &argv, options )) )
	{
		switch ( opt )
		{
			case Option_driver:
				wanted_driver_name = command::global_result.param;
				break;
			
			case Option_ramdisk:
				ramdisk_only = true;
				break;
			
			default:
				abort();
		}
	}
	
	return argv;
}


namespace tool
{
	
	namespace n = nucleus;
	namespace p7 = poseven;
	
	
	static plus::string slurp_file( p7::fd_t dirfd, const char* name )
	{
		try
		{
			n::owned< p7::fd_t > fd = p7::openat( dirfd, name, p7::o_rdonly );
			
			return p7::slurp( fd.get() );
		}
		catch ( const p7::errno_t& err )
		{
			if ( err != ENOENT )
			{
				throw;
			}
		}
		
		return "";
	}
	
	int Main( int argc, char** argv )
	{
		char *const *args = get_options( argv );
		
		const int argn = argc - (args - argv);
		
		if ( ramdisk_only )
		{
			wanted_driver_name = ".EDisk";
		}
		
		const char* vol_path = "/sys/mac/vol/list";
		
		n::shared< p7::dir_t > vol_dir = p7::opendir( vol_path );
		
		typedef p7::directory_contents_container directory_container;
		
		directory_container contents = p7::directory_contents( vol_dir );
		
		typedef directory_container::const_iterator Iter;
		
		for ( Iter it = contents.begin();  it != contents.end();  ++it )
		{
			const char* vol_name = *it;
			
			n::owned< p7::fd_t > vol_N_dirfd = p7::openat( p7::dirfd( vol_dir ), vol_name, p7::o_rdonly | p7::o_directory );
			
			bool wanted = true;
			
			if ( wanted_driver_name != NULL )
			{
				plus::string driver_name = slurp_file( vol_N_dirfd.get(), "driver/.~name" );
				
				wanted = driver_name == wanted_driver_name;
			}
			
			if ( wanted )
			{
				plus::var_string message = vol_path;
				
				message += '/';
				message += vol_name;
				message += "\n";
				
				p7::write( p7::stdout_fileno, message );
			}
		}
		
		return 0;
	}
	
}
