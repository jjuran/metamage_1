// Nitrogen/OSUtils.cp
// -------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004-2006 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NITROGEN_OSUTILS_H
#include "Nitrogen/OSUtils.h"
#endif

// Nitrogen
#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif


namespace Nitrogen
{
	
	// does nothing, but guarantees construction of theRegistration
	NUCLEUS_DEFINE_ERRORS_DEPENDENCY( DeferredTaskManager )
	
	
	static void RegisterDeferredTaskManagerErrors();
	
	
#pragma force_active on
	
	class DeferredTaskManagerErrorsRegistration
	{
		public:
			DeferredTaskManagerErrorsRegistration()  { RegisterDeferredTaskManagerErrors(); }
	};
	
	static DeferredTaskManagerErrorsRegistration theRegistration;
	
#pragma force_active reset
	
	
	void DTInstall( DeferredTask& dtTaskPtr )
	{
		ThrowOSStatus( ::DTInstall( &dtTaskPtr ) );
	}
	
	void RegisterDeferredTaskManagerErrors()
	{
		RegisterOSStatus< vTypErr >();
	}
	
}

