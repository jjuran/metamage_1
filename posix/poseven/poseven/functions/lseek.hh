// lseek.hh
// --------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_LSEEK_HH
#define POSEVEN_FUNCTIONS_LSEEK_HH

// POSIX
#include <unistd.h>

// poseven
#include "poseven/types/errno_t.hh"
#include "poseven/types/fd_t.hh"


namespace poseven
{
	
	enum seek_t
	{
		seek_set = SEEK_SET,
		seek_cur = SEEK_CUR,
		seek_end = SEEK_END,
		
		seek_t_max = nucleus::enumeration_traits< int >::max
	};
	
	inline off_t lseek( fd_t fd, off_t offset, seek_t whence = seek_set )
	{
		return throw_posix_result( ::lseek( fd, offset, whence ) );
	}
	
	inline off_t lseek( fd_t fd )
	{
		return throw_posix_result( ::lseek( fd, 0, seek_cur ) );
	}
	
}

#endif

