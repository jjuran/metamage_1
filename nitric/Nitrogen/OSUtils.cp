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
	
	DeferredTaskManagerErrorsRegistrationDependency::DeferredTaskManagerErrorsRegistrationDependency()
	{
		// does nothing, but guarantees construction of theRegistration
	}
	
	
	static void RegisterDeferredTaskManagerErrors();
	
	
	class DeferredTaskManagerErrorsRegistration
	{
		public:
			DeferredTaskManagerErrorsRegistration()  { RegisterDeferredTaskManagerErrors(); }
	};
	
	static DeferredTaskManagerErrorsRegistration theRegistration;
	
	
	void DTInstall( DeferredTask& dtTaskPtr )
	{
		ThrowOSStatus( ::DTInstall( &dtTaskPtr ) );
	}
	
	void RegisterDeferredTaskManagerErrors()
	{
		RegisterOSStatus< vTypErr >();
	}
	
}

