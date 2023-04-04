/*
	mkiconmask.cc
	-------------
*/

// POSIX
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

// more-posix
#include "more/perror.hh"

// icon-utils
#include "icon/icon.hh"
#include "icon/mask.hh"


#pragma exceptions off


#define PROGRAM  "mkiconmask"


static inline
int failure( const char* s )
{
	more::perror( PROGRAM, s );
	
	return 1;
}

static inline
int failure( const char* s1, const char* s2 )
{
	more::perror( PROGRAM, s1, s2 );
	
	return 1;
}

int main( int argc, char** argv )
{
	char* const* args = argv + (argc != 0);
	
	const char* path = args[ 0 ];
	
	if ( path == NULL )
	{
		path = "/dev/fd/0";
	}
	
	int fd = open( path, O_RDONLY );
	
	if ( fd < 0 )
	{
		return failure( path );
	}
	
	struct stat st;
	
	int nok = fstat( fd, &st );
	
	if ( nok )
	{
		return failure( path );
	}
	
	off_t length = st.st_size;
	
	if ( ! S_ISREG( st.st_mode ) )
	{
		return failure( path, "regular file expected" );
	}
	
	if ( length != 128  &&  length != 256 )
	{
		return failure( path, "unrecognized icon length" );
	}
	
	length = 128;
	
	using icon::icon32_data;
	using icon::derive_mask;
	
	icon32_data icon;
	icon32_data mask;
	
	ssize_t n_read = read( fd, &icon, length );
	
	if ( n_read < 0 )
	{
		return failure( path );
	}
	
	if ( n_read < sizeof length )
	{
		return failure( path, "incomplete read" );
	}
	
	derive_mask( icon, mask );
	
	if ( ! isatty( STDOUT_FILENO ) )
	{
		write( STDOUT_FILENO, &mask, sizeof mask );
	}
	
	return 0;
}
