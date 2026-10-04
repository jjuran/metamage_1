/*
	find_SDK_dir.cc
	---------------
*/

#include "one_path/find_SDK_dir.hh"

// Standard C
#include <stdlib.h>

// more-libc
#include "more/string.h"

// gear
#include "gear/find.hh"

// vxo
#include "vxo/errno.hh"

// poseven
#include "poseven/types/errno_t.hh"

// one_path
#include "one_path/find_MPW_dir.hh"


namespace p7 = poseven;


static const char env_sdk_dir[] = "SDK_DIR";
static const char sdk_dirname[] = "Interfaces&Libraries";


plus::string find_SDK_dir()
{
	if ( const char* sdk_dir = getenv( env_sdk_dir ) )
	{
		return sdk_dir;
	}
	
	vxo::Box xstring = find_MPW_dir();
	
	if ( const vxo::Errno* error = xstring.is< vxo::Errno >() )
	{
		throw p7::errno_t( error->get() );
	}
	
	const plus::string& pathname = *(const plus::string*) &xstring;
	
	const char* p = pathname.data();
	
	if ( const char* it = gear::find_last_match( p, pathname.size(), '/' ) )
	{
		++it;
		
		plus::string sdk_dir;
		
		char* q = sdk_dir.reset( it - p + sizeof sdk_dirname - 1 );
		
		q = (char*) mempcpy( q, p, it - p );
		q = (char*) mempcpy( q, sdk_dirname, sizeof sdk_dirname - 1 );
		
		return sdk_dir;
	}
	
	throw p7::errno_t( ENOENT );
}
