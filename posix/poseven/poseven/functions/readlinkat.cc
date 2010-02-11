/*
	readlinkat.cc
	-------------
	
	Copyright 2010, Joshua Juran.
*/

#include "poseven/functions/readlinkat.hh"

// POSIX
#include <unistd.h>

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	std::string readlinkat( fd_t dirfd, const char* path )
	{
		std::string result;
		
		size_t buffer_length = 128;
		
		ssize_t size = 0;
		
		while ( true )
		{
			result.resize( buffer_length );
			
			size = throw_posix_result( ::readlinkat( dirfd, path, &result[0], buffer_length ) );
			
			if ( size != buffer_length )
			{
				break;
			}
			
			buffer_length *= 2;
		}
		
		result.resize( size );
		
		return result;
	}
	
}

