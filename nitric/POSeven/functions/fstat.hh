// fstat.hh
// --------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_FSTAT_HH
#define POSEVEN_FUNCTIONS_FSTAT_HH

// POSIX
#include <sys/stat.h>

// Io
#include "io/io.hh"

// POSeven
#include "POSeven/types/errno_t.hh"
#include "POSeven/types/fd_t.hh"


namespace poseven
{
	
	inline struct stat fstat( fd_t fd )
	{
		struct stat stat_buffer;
		
		throw_posix_result( ::fstat( fd, &stat_buffer ) );
		
		return stat_buffer;
	}
	
}

namespace io
{
	
	inline std::size_t get_file_size( poseven::fd_t stream, overload = overload() )
	{
		struct stat status = poseven::fstat( stream );
		
		if ( !S_ISREG( status.st_mode ) )
		{
			poseven::throw_errno( ESPIPE );
		}
		
		return status.st_size;
	}
	
}

#endif

