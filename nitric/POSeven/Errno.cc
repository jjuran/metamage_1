// Errno.cc
// --------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2006-2007 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#include "POSeven/Errno.hh"

// Standard C
#include <errno.h>


namespace POSeven
{
	
	static void RegisterPOSIXErrnos();
	
	class POSIXErrnosRegistration
	{
		public:
			POSIXErrnosRegistration()  { RegisterPOSIXErrnos(); }
	};
	
	static POSIXErrnosRegistration theRegistration;
	
	
	void ThrowPOSIXResultInternalDebug( POSIXResult  result,
	                                    const char*  text,
	                                    const char*  file,
	                                    int          line )
	{
		if ( result == -1 )
		{
			ThrowErrnoInternalDebug( errno, text, file, line );
		}
	}
	
	void ThrowErrnoInternalDebug( Errno number, const char* text, const char *file, int line )
	{
		Nucleus::ThrowErrorCode< Errno >( number );
	}
	
	void ThrowPOSIXResultInternal( POSIXResult result )
	{
		if ( result == -1 )
		{
			ThrowErrnoInternal( errno );
		}
	}
	
	void ThrowErrnoInternal( Errno number )
	{
		Nucleus::ThrowErrorCode< Errno >( number );
	}
	
	void RegisterPOSIXErrnos()
	{
		RegisterErrno< ENOENT >();
		RegisterErrno< ENOMEM >();
		RegisterErrno< EAGAIN >();
	}
	
}

