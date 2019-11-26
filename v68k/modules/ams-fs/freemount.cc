/*
	freemount.cc
	------------
*/

#include "freemount.hh"

// Standard C
#include <errno.h>

// Standard C++
#include <new>

// plus
#include "plus/mac_utf8.hh"

// freemount-client
#include "freemount/synced.hh"


using plus::string;
using plus::var_string;

int try_to_get( int in, int out, const string& path, var_string& data )
{
	namespace F = freemount;
	
	try
	{
		data = F::synced_get( in, out, plus::utf8_from_mac( path ) ).move();
		
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

int try_to_put( int fd, const plus::string& path, const plus::string& data )
{
	namespace F = freemount;
	
	try
	{
		const int in  = fd;
		const int out = fd;
		
		F::synced_put( in, out, plus::utf8_from_mac( path ), data );
		
		return 0;
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
