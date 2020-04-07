/*
	mkpub.cc
	--------
*/

// POSIX
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

// Standard C
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// must
#include "must/read.h"
#include "must/write.h"

// ed25519-donna
#include "ed25519.h"

// command
#include "command/get_option.hh"


#define PROGRAM  "ed-mkpub"

#define USAGE  "usage: " PROGRAM " -i secret-key -o public-key"


enum
{
	Opt_input  = 'i',
	Opt_output = 'o',
};

static command::option options[] =
{
	{ "sec", Opt_input,  command::Param_required },
	{ "pub", Opt_output, command::Param_required },
};

static int seckey_fd = -1;
static int pubkey_fd = -1;

const unsigned seckey_size = sizeof (ed25519_secret_key);
const unsigned pubkey_size = sizeof (ed25519_public_key);

static
int fixed_size_input( const char* path, unsigned fixed_size )
{
	int fd = open( path, O_RDONLY );
	
	if ( fd < 0 )
	{
		fprintf( stderr, PROGRAM ": %s: %s\n", path, strerror( errno ) );
		exit( 1 );
	}
	
	struct stat st;
	
	int nok = fstat( fd, &st );
	
	if ( nok )
	{
		fprintf( stderr, PROGRAM ": %s: %s\n", path, strerror( errno ) );
		exit( 1 );
	}
	
	if ( S_ISREG( st.st_mode )  &&  st.st_size != fixed_size )
	{
		fprintf( stderr,
		         PROGRAM ": %s: expected size %u, not %ld\n",
		         path,
		         fixed_size,
		         (long) st.st_size );
		
		exit( 3 );
	}
	
	return fd;
}

static
int fixed_size_output( const char* path, unsigned fixed_size )
{
	int fd = open( path, O_WRONLY | O_CREAT, 0666 );
	
	if ( fd < 0 )
	{
		fprintf( stderr, PROGRAM ": %s: %s\n", path, strerror( errno ) );
		exit( 1 );
	}
	
	struct stat st;
	
	int nok = fstat( fd, &st );
	
	if ( nok )
	{
		fprintf( stderr, PROGRAM ": %s: %s\n", path, strerror( errno ) );
		exit( 1 );
	}
	
	if ( S_ISREG( st.st_mode )  &&  st.st_size  &&  st.st_size != fixed_size )
	{
		fprintf( stderr,
		         PROGRAM ": %s: expected size %u, not %lu "
		         "(delete or truncate it first)\n",
		         path,
		         fixed_size,
		         (long) st.st_size );
		
		exit( 3 );
	}
	
	return fd;
}

static
char* const* get_options( char* const* argv )
{
	int opt;
	
	++argv;  // skip arg 0
	
	while ( (opt = command::get_option( &argv, options )) > 0 )
	{
		using command::global_result;
		
		const char* path = global_result.param;
		
		switch ( opt )
		{
			case Opt_input:
				seckey_fd = fixed_size_input( path, seckey_size );
				break;
			
			case Opt_output:
				pubkey_fd = fixed_size_output( path, pubkey_size );
				break;
			
			default:
				break;
		}
	}
	
	return argv;
}

int main( int argc, char** argv )
{
	char* const* args = get_options( argv );
	
	if ( seckey_fd < 0  ||  pubkey_fd < 0 )
	{
		fprintf( stderr, "%s\n", USAGE );
		return 2;
	}
	
	ed25519_secret_key secret_key = { 0 };
	
	must_read( seckey_fd, secret_key, sizeof secret_key );
	
	ed25519_public_key public_key;
	
	ed25519_publickey( secret_key, public_key );
	
	memset( secret_key, '\0', sizeof secret_key );
	
	must_write( pubkey_fd, public_key, sizeof public_key );
	
	return 0;
}
