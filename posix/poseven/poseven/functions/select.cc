/*
	select.cc
	---------
*/

#include "poseven/functions/select.hh"

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	unsigned select( unsigned  n_fds,
	                 fd_set*   r,
	                 fd_set*   w,
	                 fd_set*   x,
	                 timeval*  tv )
	{
		return throw_posix_result( ::select( n_fds, r, w, x, tv ) );
	}
	
}

