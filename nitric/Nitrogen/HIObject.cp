// Nitrogen/HIObject.cp
// --------------------
//
// Maintained by Marshall Clow

// Part of the Nitrogen project.
//
// Written 2004-2006 by Marshall Clow and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


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
