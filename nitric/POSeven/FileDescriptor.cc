// =================
// FileDescriptor.cc
// =================

#include "POSeven/FileDescriptor.hh"

// POSIX
#include <fcntl.h>


#ifndef __MACH__
#define open(name, oflag, mode)  open( (name), (oflag) )
#endif


namespace POSeven
{
	
	Nucleus::Owned< FileDescriptor > Open( const char* name, int oflag, int mode )
	{
		// register errnos
		
		int fd;
		
		ThrowPOSIXResult( fd = ::open( name, oflag, mode ) );
		
		return Nucleus::Owned< FileDescriptor >::Seize( FileDescriptor( fd ) );
	}
	
	void Close( Nucleus::Owned< FileDescriptor > fd )
	{
		// register errnos
		
		ThrowPOSIXResult( ::close( fd.Release() ) );
	}
	
	ssize_t Read( FileDescriptor fd, char* buffer, std::size_t byteCount )
	{
		ssize_t bytesRead;
		
		ThrowPOSIXResult( bytesRead = ::read( fd, buffer, byteCount ) );
		
		if ( bytesRead == 0 )
		{
			throw io::end_of_input();
		}
		
		return bytesRead;
	}
	
	ssize_t Write( FileDescriptor fd, const char* buffer, std::size_t byteCount )
	{
		ssize_t bytesWritten;
		
		ThrowPOSIXResult( bytesWritten = ::write( fd, buffer, byteCount ) );
		
		return bytesWritten;
	}
	
	
	namespace Testing
	{
		
		static void Foo()
		{
			FileDescriptor foo = Open( "/etc/motd", O_RDONLY, 0 );
			
			//Read( foo );
			
			int fd = foo;
			
			// This is a compile error with enums!  YES!!
			//Write( fd );
		}
		
	}
	
}

