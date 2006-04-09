/*	=============
 *	StandardIO.cc
 *	=============
 */

#include "Orion/StandardIO.hh"

// Standard C/C++
#include <cerrno>

// POSIX
#include "unistd.h"

// POSeven
#include "POSeven/Errno.hh"

// Io
#include "Io/Exceptions.hh"


namespace Orion
{
	
	namespace P7 = POSeven;
	
	int Read( int fd, char* data, std::size_t byteCount )
	{
		int bytes = read( fd, data, byteCount );
		
		if ( bytes == 0 )
		{
			throw Io::EndOfInput();
		}
		else if ( bytes == -1 )
		{
			if ( errno == EWOULDBLOCK )
			{
				throw Io::NoDataPending();
			}
			
			throw P7::Errno( errno );
		}
		
		return bytes;
	}
	
	int Write( int fd, const char* data, std::size_t byteCount )
	{
		int bytes = write( fd, data, byteCount );
		
		if ( bytes == -1 )
		{
			throw P7::Errno( errno );
		}
		
		return bytes;
	}
	
}  // namespace Orion

