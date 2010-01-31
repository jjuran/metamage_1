/*
	vers.cc
	-------
	
	Copyright 2010, Joshua Juran
*/

// Standard C
#include <ctype.h>
#include <string.h>

// POSIX
#include <unistd.h>

// iota
#include "iota/decimal.hh"

// more-posix
#include "more/perror.hh"


static int usage()
{
	more::perror( "usage: vers <short> <long>", 0 );
	
	return 1;
}

static int too_long()
{
	more::perror( "vers: string must not exceed 255 bytes", 0 );
	
	return 2;
}

static int bad_version( const char* short_version, const char* problem )
{
	more::perror( "vers", short_version, problem );
	
	return 3;
}

static int populate_version_buffer( char *buffer, const char *short_version )
{
	const int major = iota::parse_unsigned_decimal( short_version );
	
	if ( major > 99 )
	{
		return bad_version( short_version, "major version exceeds 99" );
	}
	
	if ( const char* dot_1 = strchr( short_version, '.' ) )
	{
		const int minor = iota::parse_unsigned_decimal( dot_1 + 1 );
		
		if ( minor > 9 )
		{
			return bad_version( short_version, "minor version exceeds 9" );
		}
		
		if ( const char* dot_2 = strchr( dot_1 + 1, '.' ) )
		{
			const int buglevel = iota::parse_unsigned_decimal( dot_2 + 1 );
			
			if ( buglevel > 9 )
			{
				return bad_version( short_version, "buglevel exceeds 9" );
			}
			
			buffer[ 1 ] |= buglevel;
		}
		
		buffer[ 1 ] |= minor << 4;
	}
	
	buffer[ 0 ] = major / 10 << 4
	            | major % 10;
	
	const char* p = short_version;
	
	while ( *p == '.'  ||  isdigit( *p ) )
	{
		++p;
	}
	
	if ( *p == '-' )
	{
		++p;
	}
	
	buffer[ 2 ] = *p == 'd' ? 0x20
	            : *p == 'a' ? 0x40
	            : *p == 'b' ? 0x60
	            : *p == 'f' ? 0x80
	            : *p == 'r' ? 0x80
	            :             0x00;
	
	if ( buffer[ 2 ] != 0x00 )
	{
		if ( *++p  == 'c' )
		{
			++p;
		}
		
		int release_number = iota::parse_unsigned_decimal( p );
		
		if ( release_number > 255 )
		{
			return bad_version( short_version, "release number exceeds 255" );
		}
		
		buffer[ 3 ] = release_number;
	}
	
	return 0;
}

int main( int argc, char **argv )
{
	if ( argc == 1 )
	{
		return usage();
	}
	
	if ( argc > 3 )
	{
		return usage();
	}
	
	const char* short_version = argv[ 1 ];
	const char* long_version  = argv[ 2 ];
	
	const size_t short_length = strlen( short_version );
	
	if ( long_version == NULL )
	{
		long_version = short_version;
	}
	
	const size_t long_length  = argc < 3 ? short_length : strlen( long_version  );
	
	if ( short_length > 255 )
	{
		return too_long();
	}
	
	if ( long_length > 255 )
	{
		return too_long();
	}
	
	char version_buffer[ 4 + 2 + 256 + 256 ];
	
	version_buffer[ 0 ] = 0;  // major
	version_buffer[ 1 ] = 0;  // minor+bug
	version_buffer[ 2 ] = 0;  // stage code
	version_buffer[ 3 ] = 0;  // non-final release #
	
	if ( int error = populate_version_buffer( version_buffer, short_version ) )
	{
		return error;
	}
	
	*(short*) &version_buffer[ 4 ] = 0;  // country code
	
	char* p = &version_buffer[ 6 ];
	
	*p++ = short_length;
	
	memcpy( p, short_version, short_length );
	
	p += short_length;
	
	*p++ = long_length;
	
	memcpy( p, long_version, long_length );
	
	p += long_length;
	
	if ( isatty( 1 ) )
	{
		more::perror( "vers: Not writing binary data to stdout on tty\n", 0 );
		
		return 3;
	}
	
	const size_t version_length = p - version_buffer;
	
	const ssize_t n_written = ::write( STDOUT_FILENO, version_buffer, version_length );
	
	if ( n_written < 0 )
	{
		more::perror( "vers: write" );
	}
	
	return 0;
}

