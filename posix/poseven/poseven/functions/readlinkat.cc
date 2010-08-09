/*
	readlinkat.cc
	-------------
	
	Copyright 2010, Joshua Juran.
*/

#include "poseven/functions/readlinkat.hh"

// Extended API Set Part 2
#include "extended-api-set/part-2.h"

// plus
#include "plus/var_string.hh"

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	plus::string readlinkat( fd_t dirfd, const char* path )
	{
		plus::var_string result;
		
		size_t buffer_length = 128;
		
		ssize_t size = 0;
		
		while ( true )
		{
			char* p = result.reset( buffer_length );
			
			size = throw_posix_result( ::readlinkat( dirfd, path, p, buffer_length ) );
			
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

