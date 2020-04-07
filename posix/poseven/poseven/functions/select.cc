/*
	select.cc
	---------
*/

#include "poseven/functions/select.hh"

#ifdef __APPLE__
// This is needed to see select() using Mac OS X 10.2's SDK.
#include <unistd.h>
#endif

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
