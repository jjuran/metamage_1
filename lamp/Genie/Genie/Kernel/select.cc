/*	=========
 *	select.cc
 *	=========
 */

// POSIX
#include "sys/select.h"
#include "unistd.h"

// Nitrogen
#include "Nitrogen/Timer.hh"

// Pedestal
#include "Pedestal/Application.hh"

// Genie
#include "Genie/current_process.hh"
#include "Genie/FileDescriptor.hh"
#include "Genie/IO/Stream.hh"
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace Ped = Pedestal;
	
	
	static int select( int n, fd_set*  readfds,
	                          fd_set*  writefds,
	                          fd_set*  exceptfds, struct timeval* timeout )
	{
		SInt64 remaining_microseconds = 1;
		UInt64 end_microseconds;
		
		if ( timeout != NULL )
		{
			remaining_microseconds = timeout->tv_sec * 1000000
			                       + timeout->tv_usec;
			
			end_microseconds = N::Microseconds() + remaining_microseconds;
		}
		
		try
		{
			fd_table& files = current_process().FileDescriptors();
			
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
				
				Yield( true );
				
				if ( timeout )
				{
					remaining_microseconds = end_microseconds - N::Microseconds();
				}
				
				for ( int i = 0;  i != n;  ++i )
				{
					if (    FD_ISSET( i, readfds   )
					     || FD_ISSET( i, writefds  )
					     || FD_ISSET( i, exceptfds ) )
					{
						StreamHandle& stream = IOHandle_Cast< StreamHandle >( *files.at( i ).handle );
						
						unsigned int poll = stream.Poll();
						
						//if ( poll == 0 )  continue;  // Optimization
						
						if ( FD_ISSET( i, readfds )  &&  poll & kPollRead )
						{
							FD_SET( i, &rd );
							
							++result;
						}
						
						if ( FD_ISSET( i, writefds )  &&  poll & kPollWrite )
						{
							FD_SET( i, &wr );
							
							++result;
						}
						
						if ( FD_ISSET( i, exceptfds )  &&  poll & kPollExcept )
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
			return set_errno_from_exception();
		}
	}
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( select );
	
	#pragma force_active reset
	
}

