// slurp.hh
// --------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_EXTRAS_SLURP_HH
#define POSEVEN_EXTRAS_SLURP_HH

// Standard C++
#include <string>

// Io
#include "io/slurp.hh"

// Nucleus
#include "Nucleus/Flattener.h"

// POSeven
#include "POSeven/Open.hh"
#include "POSeven/functions/fstat.hh"


namespace poseven
{
	
	inline std::string slurp( const std::string& path )
	{
		return io::slurp_file< Nucleus::StringFlattener< std::string > >( path );
	}
	
	inline std::string slurp( const char* path )
	{
		return io::slurp_input< Nucleus::StringFlattener< std::string > >( open( path, o_rdonly ).get() );
	}
	
}

#endif

