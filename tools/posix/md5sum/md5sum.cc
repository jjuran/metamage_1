/*
	md5sum.cc
	---------
*/

// POSIX
#include <fcntl.h>
#include <unistd.h>

// Standard C
#include <string.h>

// more-libc
#include "more/string.h"

// more-posix
#include "more/perror.hh"

// gear
#include "gear/hexadecimal.hh"

// crypto
#include "md5/md5.hh"


#pragma exceptions off


using crypto::md5_digest;
using crypto::md5_engine;


static inline size_t min( size_t a, size_t b )
{
	return a < b ? a : b;
}

static int error( const char* path )
{
	more::perror( "md5sum", path );
	
	return 1;
}

static int md5sum( const char* path )
{
	const int input_fd = open( path, O_RDONLY );
	
	if ( input_fd < 0 )
	{
		return error( path );
	}
	
	const size_t buffer_size = 4096;
	
	char buffer[ 4096 ];
	char* block;
	ssize_t count;
	
	md5_engine engine;
	
	// loop exits on a partial block or at EOF
	while ( true )
	{
		ssize_t n_read = read( input_fd, buffer, sizeof buffer );
		
		if ( n_read < 0 )
		{
			return error( path );
		}
		
		count = n_read;
		
		block = buffer;
		
		while ( count >= 64 )
		{
			engine.digest_block( block );
			
			block += 64;
			count -= 64;
		}
		
		if ( n_read != sizeof buffer )
		{
			break;
		}
	}
	
	const md5_digest& digest = engine.finish( block, count );
	
	const size_t n_nibbles = sizeof (md5_digest) * 2;
	const size_t max_path_len = sizeof buffer - n_nibbles - 2 - 1;
	
	const size_t len = min( strlen( path ), max_path_len );
	
	char* p = gear::hexpcpy_lower( buffer, &digest, sizeof digest );
	
	*p++ = ' ';
	*p++ = ' ';
	
	p = (char*) mempcpy( p, path, len );
	
	*p++ = '\n';
	
	(void) write( STDOUT_FILENO, buffer, p - buffer );
	
	return 0;
}

int main( int argc, char** argv )
{
	bool had_errors = false;
	
	for ( int i = 1;  i < argc;  ++i )
	{
		had_errors = md5sum( argv[ i ] )  ||  had_errors;
	}
	
	return had_errors ? 1 : 0;
}
