/*
	getpass.cc
	----------
	
	Copyright 2010, Joshua Juran
*/

// Standard C
#include <errno.h>
#include <string.h>

// POSIX
#include <fcntl.h>
#include <unistd.h>

// Debug
#include "debug/assert.hh"


// Exceptions are off here
#pragma exceptions off


#define PASS_MAX  127


char* getpass( const char* prompt )
{
	ASSERT( prompt != NULL );
	
	int tty_fd = open( "/dev/tty", O_RDWR );
	
	if ( tty_fd < 0 )
	{
		if ( errno == ENOENT )
		{
			errno = ENXIO;
		}
		
		return NULL;
	}
	
	const size_t length = strlen( prompt );
	
	ssize_t n_written = write( tty_fd, prompt, length );
	
	if ( n_written < length )
	{
		if ( n_written >= 0 )
		{
			errno = EIO;
		}
		
		return NULL;
	}
	
	static char password[ PASS_MAX + 1 ];
	
	ssize_t n_read = read( tty_fd, password, sizeof password );
	
	const int read_errno = errno;
	
	(void) close( tty_fd );
	
	if ( n_read < 0 )
	{
		errno = read_errno;
		
		return NULL;
	}
	
	(void) close( tty_fd );
	
	if ( n_read == 0 )
	{
		return "";
	}
	
	char* last = &password[ n_read - 1 ];
	
	const bool no_newline = *last != '\n';
	
	if ( no_newline  &&  n_read > PASS_MAX )
	{
		errno = ERANGE;
		
		return NULL;
	}
	
	last[ no_newline ] = '\0';
	
	return password;
}
