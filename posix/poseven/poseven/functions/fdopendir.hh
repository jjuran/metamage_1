// fdopendir.hh
// ------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_FDOPENDIR_HH
#define POSEVEN_FUNCTIONS_FDOPENDIR_HH

// poseven
#include "poseven/functions/close.hh"
#include "poseven/functions/closedir.hh"


namespace poseven
{
	
	nucleus::owned< dir_t > fdopendir( nucleus::owned< fd_t > fd );
	
}

#endif

