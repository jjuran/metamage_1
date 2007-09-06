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


namespace POSeven
{
	
	static void RegisterPOSIXErrnos();
	
	class POSIXErrnosRegistration
	{
		public:
			POSIXErrnosRegistration()  { RegisterPOSIXErrnos(); }
	};
	
	static POSIXErrnosRegistration theRegistration;
	
	
	void ThrowErrno_Internal( Errno                             number,
	                          const Nucleus::DebuggingContext&  debugging )
	{
		Nucleus::ThrowErrorCode< Errno >( number, debugging );
	}
	
	void RegisterPOSIXErrnos()
	{
		RegisterErrno< ENOENT >();
		RegisterErrno< ENOMEM >();
		RegisterErrno< EAGAIN >();
	}
	
}

