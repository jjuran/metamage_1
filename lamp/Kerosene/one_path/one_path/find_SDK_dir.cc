/*
	find_SDK_dir.cc
	---------------
*/

#include "one_path/find_SDK_dir.hh"

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


plus::string find_SDK_dir()
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
