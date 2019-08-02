/*
	freemount.cc
	------------
*/

#include "freemount.hh"

// Standard C
#include <errno.h>

// Standard C++
#include <new>

// freemount-client
#include "freemount/synced.hh"


using plus::string;
using plus::var_string;

int try_to_get( int in, int out, const string& path, var_string& data )
{
	namespace F = freemount;
	
	try
	{
		data = F::synced_get( in, out, path ).move();
		
		return 0;
	}
	catch ( const std::bad_alloc& )
	{
		return -ENOMEM;
	}
	catch ( const F::path_error& e )
	{
		return -e.error;
	}
	catch ( ... )
	{
		return -EIO;
	}
}

int try_to_get( const char* begin, unsigned len, plus::var_string& data )
{
	const int in  = 6;
	const int out = 7;
	
	plus::string path( begin, len, plus::delete_never );
	
	return try_to_get( in, out, path, data );
}
