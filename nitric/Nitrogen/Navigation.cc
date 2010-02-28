// Nitrogen/Navigation.cc
// ----------------------
//
// Maintained by Marshall Clow

// Part of the Nitrogen project.
//
// Written 2004-2006 by Marshall Clow and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#include "Nitrogen/Navigation.hh"


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
	
