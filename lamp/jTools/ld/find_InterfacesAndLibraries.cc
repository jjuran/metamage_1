/*
	find_InterfacesAndLibraries.cc
	------------------------------
	
	Joshua Juran
*/

#include "find_InterfacesAndLibraries.hh"

// Standard C++
#include <algorithm>

// poseven
#include "poseven/types/errno_t.hh"

// ld
#include "find_appl.hh"


namespace tool
{
	
	namespace p7 = poseven;
	
	
	std::string find_InterfacesAndLibraries()
	{
		const char* sig_ToolServer = "MPSX";
		
		std::string pathname = find_appl( sig_ToolServer );
		
		typedef std::string::reverse_iterator Iter;
		
		Iter it = std::find( pathname.rbegin(), pathname.rend(), '/' );
		
		if ( it == pathname.rend() )
		{
			p7::throw_errno( ENOENT );
		}
		
		it = std::find( it + 1, pathname.rend(), '/' );
		
		if ( it == pathname.rend() )
		{
			p7::throw_errno( ENOENT );
		}
		
		pathname.resize( pathname.rend() - it );
		
		pathname += "Interfaces&Libraries";
		
		return pathname;
	}
	
}

