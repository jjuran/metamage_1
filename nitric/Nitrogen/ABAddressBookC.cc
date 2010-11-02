// Nitrogen/ABAddressBookC.cc
// --------------------------
//
// Maintained by Marshall Clow

// Part of the Nitrogen project.
//
// Written 2004-2006 by Marshall Clow and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifdef __MACH__

#ifndef NITROGEN_ABADDRESSBOOKC_HH
#include "Nitrogen/ABAddressBookC.hh"
#endif

namespace Nitrogen
{
	
	AddressBookErrorsRegistrationDependency::AddressBookErrorsRegistrationDependency()
	{
		// does nothing, but guarantees construction of theRegistration
	}
	
	
	static void RegisterAddressBookErrors();
	
	
	class AddressBookErrorsRegistration
	{
		public:
			AddressBookErrorsRegistration()  { RegisterAddressBookErrors(); }
	};
	
	static AddressBookErrorsRegistration theRegistration;
	
	
	void RegisterAddressBookErrors()
	{
	}
	
}

#endif

