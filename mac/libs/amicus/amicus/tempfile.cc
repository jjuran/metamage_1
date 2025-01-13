/*
	tempfile.cc
	-----------
*/

#include "amicus/tempfile.hh"

// POSIX
#include <unistd.h>

// gear
#include "gear/inscribe_decimal.hh"

// plus
#include "plus/var_string.hh"

// frontend-common
#include "frend/tempdir.hh"


namespace amicus
{

const char* tempfile_location()
{
	static plus::var_string path;
	
	path = frend::tempdir_path();
	
	path += "/amicus-";
	path += gear::inscribe_unsigned_decimal( getpid() );
	path += ".works";
	
	return path.c_str();
}

}
