// HIObject.cp

#ifndef NITROGEN_HIOBJECT_H
#include "Nitrogen/HIObject.h"
#endif

#include "Nitrogen/MacErrors.h"

namespace Nitrogen {

	HIObjectErrorsRegistrationDependency::HIObjectErrorsRegistrationDependency()
	{
		// does nothing, but guarantees construction of theRegistration
	}
	
	
	static void RegisterHIObjectErrors();
	
	
	class HIObjectErrorsRegistration
	{
		public:
			HIObjectErrorsRegistration()  { RegisterHIObjectErrors(); }
	};
	
	static HIObjectErrorsRegistration theRegistration;
	
	
  	void RegisterHIObjectErrors () {
		RegisterOSStatus< hiObjectClassExistsErr        >();
		RegisterOSStatus< hiObjectClassHasInstancesErr  >();
		RegisterOSStatus< hiObjectClassHasSubclassesErr >();
		RegisterOSStatus< hiObjectClassIsAbstractErr    >();
		}

	}
