// ABAddressBookC.cp

#ifndef NITROGEN_ADDRESSBOOKC_H
#include "Nitrogen/ABAddressBookC.h"
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

