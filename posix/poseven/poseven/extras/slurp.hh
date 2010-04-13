/*
	slurp.hh
	--------
	
	Copyright 2009, Joshua Juran
*/


#ifndef POSEVEN_EXTRAS_SLURP_HH
#define POSEVEN_EXTRAS_SLURP_HH

// Standard C++
#include <string>

// poseven
#include "poseven/types/fd_t.hh"


namespace poseven
{
	
	std::string slurp( fd_t fd );
	
	std::string slurp( const char* path );
	
}

#endif

