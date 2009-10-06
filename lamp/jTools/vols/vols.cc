/*	=======
 *	vols.cc
 *	=======
 */

// Standard C++
#include <algorithm>

// Standard C/C++
#include <cerrno>
#include <cstdio>
#include <cstring>

// POSIX
#include <unistd.h>

// Nucleus
#include "Nucleus/Shared.h"

// poseven
#include "poseven/Directory.hh"
#include "poseven/extras/slurp.hh"
#include "poseven/functions/dirfd.hh"
#include "poseven/functions/open.hh"
#include "poseven/functions/openat.hh"
#include "poseven/functions/opendir.hh"
#include "poseven/functions/write.hh"

// Orion
#include "Orion/get_options.hh"
#include "Orion/Main.hh"


namespace tool
{
	
	namespace NN = Nucleus;
	namespace p7 = poseven;
	namespace o = orion;
	
	
	int Main( int argc, iota::argv_t argv )
	{
		const char* wanted_driver_name = NULL;
		
		bool ramdisk_only = false;
		
		o::bind_option_to_variable( "--driver", wanted_driver_name );
		
		o::bind_option_to_variable( "--ram", ramdisk_only );
		
		o::get_options( argc, argv );
		
		if ( ramdisk_only )
		{
			wanted_driver_name = ".EDisk";
		}
		
		const char* vol_path = "/sys/mac/vol";
		
		NN::Shared< p7::dir_t > vol_dir = p7::opendir( vol_path );
		
		typedef p7::directory_contents_container directory_container;
		
		directory_container contents = p7::directory_contents( vol_dir );
		
		typedef directory_container::const_iterator Iter;
		
		for ( Iter it = contents.begin();  it != contents.end();  ++it )
		{
			const char* vol_name = *it;
			
			NN::Owned< p7::fd_t > vol_N_dirfd = p7::openat( p7::dirfd( vol_dir ), vol_name, p7::o_rdonly | p7::o_directory );
			
			NN::Owned< p7::fd_t > name_fd = p7::openat( vol_N_dirfd.get(), "driver/name", p7::o_rdonly | p7::o_binary );
			
			bool wanted = true;
			
			if ( wanted_driver_name != NULL )
			{
				std::string driver_name = p7::slurp( name_fd.get() );
				
				wanted = driver_name == wanted_driver_name;
			}
			
			if ( wanted )
			{
				std::string message = vol_path;
				
				message += '/';
				message += vol_name;
				message += "\n";
				
				p7::write( p7::stdout_fileno, message );
			}
		}
		
		return 0;
	}
	
}

