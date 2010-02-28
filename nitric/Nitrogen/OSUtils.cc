// Nitrogen/OSUtils.cc
// -------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004-2006 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#include "Nitrogen/OSUtils.hh"

// Nitrogen
#include "Nitrogen/OSStatus.hh"


namespace Nitrogen
{
	
	// does nothing, but guarantees construction of theRegistration
	NUCLEUS_DEFINE_ERRORS_DEPENDENCY( DeferredTaskManager )
	
	
	static void RegisterDeferredTaskManagerErrors();
	
	
#if NUCLEUS_RICH_ERRORCODES
#pragma force_active on
	
	class DeferredTaskManagerErrorsRegistration
	{
		public:
			DeferredTaskManagerErrorsRegistration()  { RegisterDeferredTaskManagerErrors(); }
	};
	
	static DeferredTaskManagerErrorsRegistration theRegistration;
	
#pragma force_active reset
#endif
	
	
	void DTInstall( DeferredTask& dtTaskPtr )
	{
		ThrowOSStatus( ::DTInstall( &dtTaskPtr ) );
	}
	
	void RegisterDeferredTaskManagerErrors()
	{
		RegisterOSStatus< vTypErr >();
	}
	
}

