/*
	slurp.hh
	--------
	
	Copyright 2009, Joshua Juran
*/


#ifndef POSEVEN_EXTRAS_SLURP_HH
#define POSEVEN_EXTRAS_SLURP_HH

// plus
#include "plus/string.hh"

// poseven
#include "poseven/types/fd_t.hh"


namespace poseven
{
	
	plus::string slurp( fd_t fd );
	
	plus::string slurp( const char* path );
	
}

#endif

