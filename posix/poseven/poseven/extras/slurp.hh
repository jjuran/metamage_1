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
#ifndef POSEVEN_TYPES_FD_T_HH
#include "poseven/types/fd_t.hh"
#endif


namespace poseven
{
	
	plus::string slurp( fd_t fd );
	
	plus::string slurp( const char* path );
	
}

#endif

