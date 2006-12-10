//	Navigation.h
#ifndef NITROGEN_NAVIGATION_H
#include "Nitrogen/Navigation.h"
#endif

namespace Nitrogen {

	NavServicesErrorsRegistrationDependency::NavServicesErrorsRegistrationDependency()
	{
		// does nothing, but guarantees construction of theRegistration
	}
	
	
	static void RegisterNavServicesErrors();
	
	
	class NavServicesErrorsRegistration
	{
		public:
			NavServicesErrorsRegistration()  { RegisterNavServicesErrors(); }
	};
	
	static NavServicesErrorsRegistration theRegistration;
	
	
	void RegisterNavServicesErrors () {
		RegisterOSStatus< kNavWrongDialogStateErr            >();
		RegisterOSStatus< kNavWrongDialogClassErr            >();
		RegisterOSStatus< kNavInvalidSystemConfigErr         >();
		RegisterOSStatus< kNavCustomControlMessageFailedErr  >();
		RegisterOSStatus< kNavInvalidCustomControlMessageErr >();
		RegisterOSStatus< kNavMissingKindStringErr           >();
		}

	}
	
