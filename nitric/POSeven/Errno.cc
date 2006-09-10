// ========
// Errno.cc
// ========

#include "POSeven/Errno.hh"

// Standard C
#include <errno.h>


namespace POSeven
{
	
	static ErrnoLoggingProc gErrnoLoggingProc = NULL;
	
	ErrnoLoggingProc SetErrnoLoggingProc( ErrnoLoggingProc newProc )
	{
		ErrnoLoggingProc retVal = gErrnoLoggingProc;
		gErrnoLoggingProc = newProc;
		
		return retVal;
	}
	
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
		if ( gErrnoLoggingProc != NULL )
		{
			try
			{
				gErrnoLoggingProc( number, text, file, line );
			}
			catch ( ... ) {}
		}
		
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
		Nucleus::ThrowErrorCode< Errno >( errno );
	}
	
}

