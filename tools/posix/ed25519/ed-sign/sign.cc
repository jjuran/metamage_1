/*
	sign.cc
	-------
*/

// POSIX
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

// Standard C
#include <errno.h>
#include <stdio.h>
#include <string.h>

// must
#include "must/read.h"
#include "must/write.h"

// ed25519-donna
#include "ed25519.h"

// command
#include "command/get_option.hh"


#define PROGRAM  "ed-sign"

#define USAGE  "usage: " PROGRAM " -k keyfile -m msgfile -s sigfile"


enum
{
	Opt_keyfile = 'k',
	Opt_msgfile = 'm',
	Opt_sigfile = 's',
};

static command::option options[] =
{
	{ "key", Opt_keyfile, command::Param_required },
	{ "msg", Opt_msgfile, command::Param_required },
	{ "sig", Opt_sigfile, command::Param_required },
};

struct mem_ref
{
	void*   ptr;
	size_t  len;
};

static mem_ref m = { NULL, 0 };

static int seckey_fd    = -1;
static int signature_fd = -1;

const unsigned seckey_size    = sizeof (ed25519_secret_key);
const unsigned signature_size = sizeof (ed25519_signature);

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
mem_ref map_file( const char* path )
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
	
	mem_ref result;
	
	result.ptr = mmap( NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0 );
	result.len = st.st_size;
	
	if ( result.ptr == NULL )
	{
		fprintf( stderr, PROGRAM ": %s: %s\n", path, strerror( errno ) );
		exit( 1 );
	}
	
	close( fd );
	
	return result;
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
			case Opt_keyfile:
				seckey_fd = fixed_size_input( path, seckey_size );
				break;
			
			case Opt_sigfile:
				signature_fd = fixed_size_output( path, signature_size );
				break;
			
			case Opt_msgfile:
				m = map_file( path );
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
	
	if ( seckey_fd < 0  ||  signature_fd < 0 )
	{
		fprintf( stderr, "%s\n", USAGE );
		return 2;
	}
	
	ed25519_secret_key secret_key = { 0 };
	
	must_read( seckey_fd, secret_key, sizeof secret_key );
	
	ed25519_public_key public_key;
	
	ed25519_publickey( secret_key, public_key );
	
	const unsigned char* msg = (const unsigned char*) m.ptr;
	
	ed25519_signature sig;
	
	ed25519_sign( msg, m.len, secret_key, public_key, sig );
	
	memset( secret_key, '\0', sizeof secret_key );
	
	must_write( signature_fd, sig, sizeof sig );
	
	return 0;
}
