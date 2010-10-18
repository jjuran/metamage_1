/*
	write_all.cc
	------------
	
	Copyright 2009, Joshua Juran
*/


#include "poseven/extras/write_all.hh"

// poseven
#include "poseven/functions/write.hh"
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	ssize_t write_some( fd_t fd, const char* buffer, size_t n_bytes )
	{
		ssize_t result;
		
	retry:
		
		result = ::write( fd, buffer, n_bytes );
		
		if ( result == -1 )
		{
			if ( errno == EINTR )
			{
				goto retry;
			}
			
			throw_errno_internal( errno );
		}
		
		return result;
	}
	
	ssize_t write_all( fd_t fd, const char* buffer, size_t n_bytes )
	{
		const size_t n_requested = n_bytes;
		
		do
		{
			const ssize_t n_written = write_some( fd, buffer, n_bytes );
			
			buffer  += n_written;
			n_bytes -= n_written;
		}
		while ( n_bytes != 0 );
		
		return n_requested;
	}
	
}

