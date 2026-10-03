/*
	load_file.hh
	------------
*/

#ifndef FREND_LOADFILE_HH
#define FREND_LOADFILE_HH

// plus
#include "plus/string_fwd.hh"


namespace frend
{

plus::string load_file( int dirfd, const char* path );

}

#endif
