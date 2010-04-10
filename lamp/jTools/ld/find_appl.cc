/*
	find_appl.cc
	------------
	
	Joshua Juran
*/

#include "find_appl.hh"

// iota
#include "iota/decimal.hh"
#include "iota/strings.hh"

// poseven
#include "poseven/functions/open.hh"
#include "poseven/functions/openat.hh"
#include "poseven/functions/readlinkat.hh"


namespace tool
{
	
	namespace n = nucleus;
	namespace p7 = poseven;
	
	
	std::string find_appl( const char* appl )
	{
		char ram_path[] = "ram/dt/appls/APPL/latest";
		
		memcpy( ram_path + STRLEN( "ram/dt/appls/" ), appl, STRLEN( "APPL" ) );
		
		const char* path = ram_path + STRLEN( "ram/" );
		
		n::owned< p7::fd_t > vol = p7::open( "/sys/mac/vol/", p7::o_rdonly | p7::o_directory );
		
		try
		{
			return p7::readlinkat( vol, ram_path );
		}
		catch ( const p7::errno_t& err )
		{
			if ( err != ENOENT  &&  err != EINVAL )
			{
				throw;
			}
		}
		
		n::owned< p7::fd_t > vol_list = p7::openat( vol, "list", p7::o_rdonly | p7::o_directory );
		
		for ( int i = 1;  ;  ++i )
		{
			const char *name = iota::inscribe_decimal( i );
			
			n::owned< p7::fd_t > list_i = p7::openat( vol_list, name, p7::o_rdonly | p7::o_directory );
			
			try
			{
				return p7::readlinkat( list_i, path );
			}
			catch ( const p7::errno_t& err )
			{
				if ( err != ENOENT )
				{
					throw;
				}
			}
		}
	}
	
}

