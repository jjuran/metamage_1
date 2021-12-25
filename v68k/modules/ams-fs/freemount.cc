/*
	freemount.cc
	------------
*/

#include "freemount.hh"

// Standard C
#include <errno.h>

// Standard C++
#include <new>

// chars
#include "conv/mac_utf8.hh"

// plus
#include "plus/mac_utf8.hh"

// freemount-client
#include "freemount/synced.hh"

// ams-common
#include "scoped_zone.hh"


using plus::string;
using plus::utf8_from_mac;
using plus::var_string;

int try_to_get( int in, int out, const string& path, var_string& data )
{
	namespace F = freemount;
	
	scoped_zone null_zone;
	
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
	
	plus::string path( begin, len, vxo::delete_never );
	
	return try_to_get( in, out, path, data );
}

int try_to_put( int fd, const plus::string& path, const plus::string& data )
{
	namespace F = freemount;
	
	scoped_zone null_zone;
	
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

int try_to_write( int                  fd,
                  const plus::string&  path,
                  const plus::string&  data,
                  unsigned             offset )
{
	namespace F = freemount;
	
	scoped_zone null_zone;
	
	try
	{
		const int in  = fd;
		const int out = fd;
		
		F::synced_pwrite( in, out, plus::utf8_from_mac( path ), data, offset );
		
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

static
void per_dirent( const char* name, uint32_t size, void* x )
{
	var_string& data = *(var_string*) x;
	
	const unsigned mac_size = conv::sizeof_mac_from_utf8( name, name + size );
	
	const int max_len = dirent_size - 1;
	
	char buffer[ 1 + max_len ];
	
	if ( mac_size <= max_len )
	{
		char* p = buffer;
		
		*p++ = mac_size;
		
		conv::mac_from_utf8_nothrow( p, mac_size, name, size );
		
		data.append( buffer, 1 + mac_size );
		data.resize( data.size() + max_len - mac_size );
	}
}

int try_to_list( int in, int out, const string& path, var_string& data )
{
	namespace F = freemount;
	
	scoped_zone null_zone;
	
	try
	{
		data.clear();
		
		F::synced_dir( in, out, utf8_from_mac( path ), &per_dirent, &data );
		
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
