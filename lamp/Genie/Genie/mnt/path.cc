/*
	path.cc
	-------
*/

#include "Genie/mnt/path.hh"

// POSIX
#include <unistd.h>

// Standard C
#include <stdlib.h>

// gear
#include "gear/inscribe_decimal.hh"

// plus
#include "plus/var_string.hh"


namespace Genie {

static
plus::string make_socket_path()
{
	plus::var_string path;
	
	if ( const char* home = getenv( "HOME" ) )
	{
		path = home;
	}
	
	path += FS_SUBPATH "/gui-";
	
	path += gear::inscribe_unsigned_decimal( getpid() );
	
	path += ".socket";
	
	return path.move();
}

const char* socket_path()
{
	static plus::string path = make_socket_path();
	
	return path.c_str();
}

}  // namespace Genie
