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

// poseven
#include "poseven/FileDescriptor.hh"
#include "poseven/functions/fstat.hh"
#include "poseven/functions/open.hh"


namespace poseven
{
	
	inline std::string slurp( fd_t fd )
	{
		return io::slurp_input< Nucleus::StringFlattener< std::string > >( fd );
	}
	
	inline std::string slurp( const char* path )
	{
		return slurp( open( path, o_rdonly ) );
	}
	
	inline std::string slurp( const std::string& path )
	{
		return slurp( path.c_str() );
	}
	
}

#endif

