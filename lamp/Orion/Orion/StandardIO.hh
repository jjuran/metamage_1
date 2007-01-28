/*	=============
 *	StandardIO.hh
 *	=============
 */

#ifndef ORION_STANDARDIO_HH
#define ORION_STANDARDIO_HH

// POSeven
#include "POSeven/FileDescriptor.hh"

// Io
#include "Io/Stream.hh"


namespace Orion
{
	
	int Read ( int fd,       char* data, std::size_t byteCount );
	int Write( int fd, const char* data, std::size_t byteCount );
	
}

namespace Io
{
	
	typedef POSeven::FileDescriptor FD;
	
	static const FD in  = FD( 0 );
	static const FD out = FD( 1 );
	static const FD err = FD( 2 );
	
	static Stream< FD > In ( in  ),
	                    Out( out ),
	                    Err( err );
	
	inline int Read( FD fd, char* data, std::size_t byteCount )
	{
		return Orion::Read( fd, data, byteCount );
	}
	
	inline int Write( FD fd, const char* data, std::size_t byteCount )
	{
		return Orion::Write( fd, data, byteCount );
	}
	
	struct FD_Details
	{
		typedef int Storage;
		typedef FD  Value;
		
		static void SetBlocking( FD fd, bool blocking )
		{
			
		}
		
		static int Read( FD fd, char* data, std::size_t byteCount )
		{
			return Orion::Read( fd, data, byteCount );
		}
		
		static int Write( FD fd, const char* data, std::size_t byteCount )
		{
			return Orion::Write( fd, data, byteCount );
		}
	};
	
}

#endif

