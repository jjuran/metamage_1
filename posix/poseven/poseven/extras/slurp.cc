/*
	slurp.cc
	--------
	
	Copyright 2009, Joshua Juran
*/


#include "poseven/extras/slurp.hh"

// plus
#include "plus/string.hh"

// poseven
#include "poseven/extras/read_all.hh"
#include "poseven/functions/fstat.hh"
#include "poseven/functions/open.hh"


namespace poseven
{
	
	plus::string slurp( fd_t fd )
	{
		const size_t size = fstat( fd ).st_size;
		
		plus::string result;
		
		char* p = result.reset( size );
		
		const ssize_t n_read = read_all( fd, p, size );
		
		return result.substr( 0, n_read );
	}
	
	plus::string slurp( const char* path )
	{
		return slurp( open( path, o_rdonly ) );
	}
	
}
