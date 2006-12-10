/*	==========
 *	OSUtils.cp
 *	==========
 */

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

