// FileDescriptor.cc
// -----------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2006-2007 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#include "POSeven/FileDescriptor.hh"


namespace POSeven
{
	
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
	
}

