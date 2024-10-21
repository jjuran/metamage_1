/*
	read_all.cc
	-----------
	
	Copyright 2009, Joshua Juran
*/


#include "poseven/extras/read_all.hh"

// poseven
#include "poseven/functions/read.hh"
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	ssize_t read_some( fd_t fd, char* buffer, size_t n_bytes )
	{
		ssize_t result;
		
	retry:
		
		result = ::read( fd, buffer, n_bytes );
		
		if ( result < 0 )
		{
			if ( errno == EINTR )
			{
				goto retry;
			}
			
			throw_errno_internal( errno );
		}
		
		return result;
	}
	
	ssize_t read_all( fd_t fd, char* buffer, size_t n_bytes )
	{
		const size_t n_requested = n_bytes;
		
		do
		{
			const ssize_t n_read = read_some( fd, buffer, n_bytes );
			
			if ( n_read == 0 )
			{
				break;
			}
			
			buffer  += n_read;
			n_bytes -= n_read;
		}
		while ( n_bytes != 0 );
		
		return n_requested - n_bytes;
	}
	
}
