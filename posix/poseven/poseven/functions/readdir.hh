// readdir.hh
// ----------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2007 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_READDIR_HH
#define POSEVEN_FUNCTIONS_READDIR_HH

// poseven
#ifndef POSEVEN_TYPES_DIR_T_HH
#include "poseven/types/dir_t.hh"
#endif


namespace poseven
{
	
	const dirent& readdir( dir_t dir );
	
}

#endif

