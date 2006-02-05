/*	=========
 *	select.cc
 *	=========
 */

// Standard C
#include <errno.h>

// POSIX
#include "sys/select.h"
#include "unistd.h"

// Nitrogen / Carbon
#include "Nitrogen/Timer.h"

// POSeven
#include "POSeven/Errno.h"

// Genie
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/Yield.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace P7 = POSeven;
	
	
	static int select( int n, fd_set*  readfds,
	                          fd_set*  writefds,
	                          fd_set*  exceptfds, struct timeval* timeout )
	{
		UInt64 timeToBail = timeout == NULL ? UInt64( -1 )
		                                    : N::Microseconds() + timeout->tv_sec * 1000000
		                                                        + timeout->tv_usec;
		
		try
		{
			FileDescriptorMap& files = CurrentProcess().FileDescriptors();
			
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
				Yield();
				
				for ( int i = 0;  i != n;  ++i )
				{
					if (    FD_ISSET( i, readfds   )
					     || FD_ISSET( i, writefds  )
					     || FD_ISSET( i, exceptfds ) )
					{
						if ( !files.count( i ) )
						{
							return CurrentProcess().SetErrno( EBADF );
						}
						
						IORef ref = files[ i ].handle;
						
						unsigned int poll = ref.Poll();
						
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
			while ( result == 0  &&  N::Microseconds() < timeToBail );
			
			if ( n > 0 )
			{
				*readfds   = rd;
				*writefds  = wr;
				*exceptfds = ex;
			}
			
			return result;
		}
		catch ( P7::Errno& error )
		{
			CurrentProcess().SetErrno( error );
		}
		catch ( ... )
		{
			// FIXME:  Is there a better course of action, or at least a better default?
			CurrentProcess().SetErrno( EIO );
		}
		
		return -1;
	}
	
	REGISTER_SYSTEM_CALL( select );
	
}

#pragma export on
	
	int select( int n, fd_set*  readfds,
	                   fd_set*  writefds,
	                   fd_set*  exceptfds, struct timeval* timeout )
	{
		return Genie::select( n, readfds, writefds, exceptfds, timeout );
	}
	
#pragma export reset

