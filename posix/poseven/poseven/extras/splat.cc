/*
	splat.cc
	--------
	
	Copyright 2009, Joshua Juran
*/


#include "poseven/extras/splat.hh"

// poseven
#include "poseven/functions/open.hh"


namespace poseven
{
	
	void splat( const char* path, const char* buffer, size_t length )
	{
		splat( open( path, o_wronly | o_trunc ), buffer, length );
	}
	
}
