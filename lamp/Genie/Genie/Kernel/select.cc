/*	=========
 *	select.cc
 *	=========
 */

// Standard C
#include <stdint.h>
#include <time.h>

// POSIX
#include "sys/select.h"
#include "unistd.h"

// vfs
#include "vfs/file_descriptor.hh"
#include "vfs/enum/poll_result.hh"
#include "vfs/filehandle/primitives/poll.hh"

// relix-kernel
#include "relix/api/errno.hh"
#include "relix/api/get_fd_handle.hh"

// Pedestal
#include "Pedestal/Application.hh"

// Genie
#include "Genie/api/yield.hh"
#include "Genie/SystemCallRegistry.hh"


namespace Genie
{
	
	namespace Ped = Pedestal;
	
	
	static int select( int n, fd_set*  readfds,
	                          fd_set*  writefds,
	                          fd_set*  exceptfds, struct timeval* timeout )
	{
		int64_t   remaining_microseconds = 1;
		uint64_t  end_microseconds       = 0;
		
		if ( timeout != NULL )
		{
			remaining_microseconds = timeout->tv_sec * 1000000
			                       + timeout->tv_usec;
			
			end_microseconds = clock() + remaining_microseconds;
		}
		
		try
		{
			// Output fd sets
			fd_set rd, wr, ex;
			
			FD_ZERO( &rd );
			FD_ZERO( &wr );
			FD_ZERO( &ex );
			
			// For any omitted sets, we'll pretend they were zeroed.
			
			if ( readfds   == NULL )  { readfds   = &rd; }
			if ( writefds  == NULL )  { writefds  = &wr; }
			if ( exceptfds == NULL )  { exceptfds = &ex; }
			
			int result = 0;
			
			do
			{
				if ( timeout )
				{
					Ped::AdjustSleepForTimer( remaining_microseconds * 60 / 1000000 );
				}
				
				try_again( false );
				
				if ( timeout )
				{
					remaining_microseconds = end_microseconds - clock();
				}
				
				for ( int i = 0;  i != n;  ++i )
				{
					if (    FD_ISSET( i, readfds   )
					     || FD_ISSET( i, writefds  )
					     || FD_ISSET( i, exceptfds ) )
					{
						unsigned int poll = vfs::poll( relix::get_fd_handle( i ) );
						
						//if ( poll == 0 )  continue;  // Optimization
						
						if ( FD_ISSET( i, readfds )  &&  poll & vfs::Poll_read )
						{
							FD_SET( i, &rd );
							
							++result;
						}
						
						if ( FD_ISSET( i, writefds )  &&  poll & vfs::Poll_write )
						{
							FD_SET( i, &wr );
							
							++result;
						}
						
						if ( FD_ISSET( i, exceptfds )  &&  poll & vfs::Poll_except )
						{
							FD_SET( i, &ex );
							
							++result;
						}
					}
				}
			}
			while ( result == 0  &&  remaining_microseconds > 0 );
			
			if ( n > 0 )
			{
				*readfds   = rd;
				*writefds  = wr;
				*exceptfds = ex;
			}
			
			return result;
		}
		catch ( ... )
		{
			return relix::set_errno_from_exception();
		}
	}
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( select );
	
	#pragma force_active reset
	
}

