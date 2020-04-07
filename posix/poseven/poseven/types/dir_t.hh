// dir_t.hh
// --------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2007 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_TYPES_DIR_T_HH
#define POSEVEN_TYPES_DIR_T_HH

// POSIX
#ifdef __APPLE__
// Include <sys/types.h> before <dirent.h> to compile with Mac OS X 10.2's SDK.
#include <sys/types.h>
#endif
#include <dirent.h>


namespace poseven
{
	
	typedef DIR* dir_t;
	
}

#endif
