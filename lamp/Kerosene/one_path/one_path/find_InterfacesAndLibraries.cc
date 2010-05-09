/*
	find_InterfacesAndLibraries.cc
	------------------------------
	
	Joshua Juran
*/

#include "one_path/find_InterfacesAndLibraries.hh"

// Standard C
#include <stdlib.h>

// iota
#include "iota/find.hh"

// plus
#include "plus/var_string.hh"

// poseven
#include "poseven/types/errno_t.hh"

// one_path
#include "one_path/find_appl.hh"


namespace tool
{
	
	namespace p7 = poseven;
	
	
	plus::string find_InterfacesAndLibraries()
	{
		if ( const char* interfaces_libraries = getenv( "INTERFACES_LIBRARIES" ) )
		{
			return interfaces_libraries;
		}
		
		const char* sig_ToolServer = "MPSX";
		
		plus::var_string pathname = find_appl( sig_ToolServer );
		
		const char* p = pathname.data();
		
		if ( const char* it = iota::find_last_match( p, pathname.size(), '/' ) )
		{
			if (( it = iota::find_last_match( p, it - p, '/' ) ))
			{
				pathname.resize( it + 1 - p );
				
				pathname += "Interfaces&Libraries";
				
				return pathname;
			}
		}
		
		throw p7::errno_t( ENOENT );
	}
	
}

