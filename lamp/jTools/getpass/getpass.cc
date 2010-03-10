/*
	getpass.cc
	----------
	
	Copyright 2010, Joshua Juran
*/

// Standard C
#include <string.h>

// POSIX
#include <unistd.h>

// more-posix
#include "more/perror.hh"


int main( int argc, const char *const *argv )
{
	const char* prompt = argc > 1 ? argv[1] : "Password: ";
	
	char* password = getpass( prompt );
	
	if ( password == NULL )
	{
		more::perror( "getpass" );
	}
	
	ssize_t length = strlen( password );
	
	password[ length++ ] = '\n';
	
	(void) write( STDOUT_FILENO, password, length );
	
	return 0;
}

