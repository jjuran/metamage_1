/*
	more/perror.hh
	--------------
	
	Copyright 2009 Joshua Juran
*/

#ifndef MORE_PERROR_HH
#define MORE_PERROR_HH

// Standard C
#include <errno.h>


namespace more
{
	
	void perror( int errnum = errno );
	
	void perror( const char* s, int errnum = errno );
	
	void perror( const char* s1, const char* s2, int errnum = errno );
	
	void perror( const char* s1, const char* s2, const char* s3 );
	
}

#endif

