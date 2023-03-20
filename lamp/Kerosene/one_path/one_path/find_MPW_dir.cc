/*
	find_MPW_dir.cc
	-----------------
*/

#include "one_path/find_MPW_dir.hh"

// Standard C
#include <stdlib.h>

// gear
#include "gear/find.hh"

// poseven
#include "poseven/types/errno_t.hh"

// one_path
#include "one_path/find_appl.hh"


namespace p7 = poseven;


plus::string find_MPW_dir()
{
	if ( const char* mpw_dir = getenv( "MPW_DIR" ) )
	{
		return mpw_dir;
	}
	
	plus::string path = find_appl( "MPSX" );
	
	const char* p = path.data();
	
	if ( const char* it = gear::find_last_match( p, path.size(), '/' ) )
	{
		return path.substr( 0, it - p );
	}
	
	throw p7::errno_t( ENOENT );
}
