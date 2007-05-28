// =======
// Open.cc
// =======

#include "POSeven/Open.hh"

// POSIX
#include <fcntl.h>


namespace POSeven
{
	
	Nucleus::Owned< FileDescriptor > Open( const char* name, int oflag, int mode )
	{
		// register errnos
		
		int fd;
		
		ThrowPOSIXResult( fd = ::open( name, oflag, mode ) );
		
		return Nucleus::Owned< FileDescriptor >::Seize( FileDescriptor( fd ) );
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

