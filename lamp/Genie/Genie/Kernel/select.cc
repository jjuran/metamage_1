/*	=========
 *	select.cc
 *	=========
 */

// POSIX
#include "sys/select.h"
#include "unistd.h"

// Nitrogen
#include "Nitrogen/Timer.h"

// Genie
#include "Genie/IO/Stream.hh"
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"


namespace Genie
{
	
	DECLARE_MODULE_INIT( Kernel_select )
	DEFINE_MODULE_INIT(  Kernel_select )
	
	namespace N = Nitrogen;
	
	
	static int select( int n, fd_set*  readfds,
	                          fd_set*  writefds,
	                          fd_set*  exceptfds, struct timeval* timeout )
	{
		SystemCallFrame frame( "select" );
		
		UInt64 timeToBail = timeout == NULL ? UInt64( -1 )
		                                    : N::Microseconds() + timeout->tv_sec * 1000000
		                                                        + timeout->tv_usec;
		
		try
		{
			FileDescriptorMap& files = frame.Caller().FileDescriptors();
			
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
				Yield( kInterruptUnlessRestarting );
				
				for ( int i = 0;  i != n;  ++i )
				{
					if (    FD_ISSET( i, readfds   )
					     || FD_ISSET( i, writefds  )
					     || FD_ISSET( i, exceptfds ) )
					{
						if ( !files.count( i ) )
						{
							return frame.SetErrno( EBADF );
						}
						
						StreamHandle& stream = IOHandle_Cast< StreamHandle >( *files[ i ].handle );
						
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
			while ( result == 0  &&  N::Microseconds() < timeToBail );
			
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
			return frame.SetErrnoFromException();
		}
	}
	
	REGISTER_SYSTEM_CALL( select );
	
}

