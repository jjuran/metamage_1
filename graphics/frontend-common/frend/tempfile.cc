/*
	tempfile.cc
	-----------
*/

#include "frend/tempfile.hh"

// POSIX
#include <unistd.h>

// gear
#include "gear/inscribe_decimal.hh"

// plus
#include "plus/var_string.hh"

// frontend-common
#include "frend/tempdir.hh"


namespace frend
{

const char* tempfile_location()
{
	static plus::var_string path;
	
	path = tempdir_path();
	
	path += "/displayfs-";
	path += gear::inscribe_unsigned_decimal( getpid() );
	path += ".works";
	
	return path.c_str();
}

}
