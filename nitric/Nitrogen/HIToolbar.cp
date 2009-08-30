// Nitrogen/HIToolbar.cp
// ---------------------
//
// Maintained by Marshall Clow

// Part of the Nitrogen project.
//
// Written 2004-2006 by Marshall Clow and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifdef __MACH__

#include "Nitrogen/HIToolbar.h"


namespace Nitrogen {

	/*
	HIToolbarErrorsRegistrationDependency::HIToolbarErrorsRegistrationDependency()
	{
		// does nothing, but guarantees construction of theRegistration
	}
	*/
	
	
	static void RegisterHIToolbarErrors();
	
	
	class HIToolbarErrorsRegistration
	{
		public:
			HIToolbarErrorsRegistration()  { RegisterHIToolbarErrors(); }
	};
	
	static HIToolbarErrorsRegistration theRegistration;
	
	
	void RegisterHIToolbarErrors () {
	//	Apple hasn't documented any!!
		}
	
	}

#endif

