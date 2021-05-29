/*
	find_InterfacesNLibraries.cc
	----------------------------
	
	(With ccache and a slightly longer name that's consistent with the
	header, we get "find_InterfacesAndLibraries.cc.ii" -- which is 33
	characters, longer than HFS can handle.  That gets automatically clipped
	to 31 characters: "find_InterfacesAndLibraries.cc.", which gets another
	three characters (which are supposed to be ".ii") blindly trimmed from
	it, yielding "find_InterfacesAndLibraries.", which Metrowerks' compiler
	doesn't recognize as source (but treats as such anyway, with a warning).
	
	To avoid the confusion, we just shorten the name as above.
*/

#include "one_path/find_InterfacesAndLibraries.hh"

// Standard C
#include <stdlib.h>

// gear
#include "gear/find.hh"

// plus
#include "plus/var_string.hh"

// poseven
#include "poseven/types/errno_t.hh"

// one_path
#include "one_path/find_appl.hh"


namespace p7 = poseven;


static const char env_intfs_libs[] = "INTERFACES_LIBRARIES";
static const char txt_intfs_libs[] = "Interfaces&Libraries";
static const char sig_ToolServer[] = "MPSX";


plus::string find_InterfacesAndLibraries()
{
	if ( const char* interfaces_libraries = getenv( env_intfs_libs ) )
	{
		return interfaces_libraries;
	}
	
	plus::var_string pathname = find_appl( sig_ToolServer ).move();
	
	const char* p = pathname.data();
	
	if ( const char* it = gear::find_last_match( p, pathname.size(), '/' ) )
	{
		if (( it = gear::find_last_match( p, it - p, '/' ) ))
		{
			pathname.resize( it + 1 - p );
			
			pathname += txt_intfs_libs;
			
			return pathname.move();
		}
	}
	
	throw p7::errno_t( ENOENT );
}
