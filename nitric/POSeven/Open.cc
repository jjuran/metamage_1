// Open.cc
// -------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2006-2007 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


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

