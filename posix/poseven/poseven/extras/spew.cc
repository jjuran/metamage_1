/*
	spew.cc
	-------
	
	Copyright 2009, Joshua Juran
*/


#include "poseven/extras/spew.hh"

// poseven
#include "poseven/functions/open.hh"


namespace poseven
{
	
	void spew( const char* path, const char* buffer, size_t length )
	{
		spew( open( path, o_wronly | o_trunc ), buffer, length );
	}
	
}

