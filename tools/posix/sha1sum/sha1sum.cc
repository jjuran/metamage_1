/*
	sha1sum.cc
	----------
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
#include "sha1/sha1.hh"


#pragma exceptions off


using crypto::sha1_digest;
using crypto::sha1_engine;


static inline size_t min( size_t a, size_t b )
{
	return a < b ? a : b;
}

static int error( const char* path )
{
	more::perror( "sha1sum", path );
	
	return 1;
}

static int sha1sum( const char* path )
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
	
	sha1_engine engine;
	
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
	
	const sha1_digest& digest = engine.finish( block, count );
	
	const size_t n_nibbles = sizeof (sha1_digest) * 2;
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
		had_errors = sha1sum( argv[ i ] )  ||  had_errors;
	}
	
	return had_errors ? 1 : 0;
}
