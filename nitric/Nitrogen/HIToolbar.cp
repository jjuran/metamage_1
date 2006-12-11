// HIToolbar.cp

#ifndef NITROGEN_HITOOLBAR_H
#include "Nitrogen/HIToolbar.h"
#endif

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