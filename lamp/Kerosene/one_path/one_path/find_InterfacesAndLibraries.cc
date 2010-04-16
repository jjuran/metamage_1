/*
	find_InterfacesAndLibraries.cc
	------------------------------
	
	Joshua Juran
*/

#include "one_path/find_InterfacesAndLibraries.hh"

// iota
#include "iota/find.hh"

// poseven
#include "poseven/types/errno_t.hh"

// one_path
#include "one_path/find_appl.hh"


namespace tool
{
	
	namespace p7 = poseven;
	
	
	std::string find_InterfacesAndLibraries()
	{
		const char* sig_ToolServer = "MPSX";
		
		std::string pathname = find_appl( sig_ToolServer );
		
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

