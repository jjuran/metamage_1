/*
	slurp.cc
	--------
	
	Copyright 2009, Joshua Juran
*/


#include "poseven/extras/slurp.hh"

// poseven
#include "poseven/extras/read_all.hh"
#include "poseven/functions/fstat.hh"
#include "poseven/functions/open.hh"


namespace poseven
{
	
	std::string slurp( fd_t fd )
	{
		const size_t size = fstat( fd ).st_size;
		
		std::string result;
		
		result.resize( size );
		
		const ssize_t n_read = read_all( fd, &result[0], size );
		
		result.resize( n_read );
		
		return result;
	}
	
	std::string slurp( const char* path )
	{
		return slurp( open( path, o_rdonly ) );
	}
	
}

