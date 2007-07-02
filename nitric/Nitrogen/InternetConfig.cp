// Nitrogen/InternetConfig.cp
// --------------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004-2006 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NITROGEN_INTERNETCONFIG_H
#include "Nitrogen/InternetConfig.h"
#endif

#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif

namespace Nitrogen
{
	
	InternetConfigErrorsRegistrationDependency::InternetConfigErrorsRegistrationDependency()
	{
		// does nothing, but guarantees construction of theRegistration
	}
	
	
	static void RegisterInternetConfigErrors();
	
	
	class InternetConfigErrorsRegistration
	{
		public:
			InternetConfigErrorsRegistration()  { RegisterInternetConfigErrors(); }
	};
	
	static InternetConfigErrorsRegistration theRegistration;
	
	
	Nucleus::Owned< Handle > ICFindPrefHandle( ICInstance        instance,
	                                  ConstStr255Param  key,
	                                  ICAttr&           attr )
	{
		Nucleus::Owned< Handle > prefh = NewHandle( 0 );
		ThrowOSStatus( ::ICFindPrefHandle( instance, key, &attr, prefh.Get() ) );
		return prefh;
	}
	
	Nucleus::Owned< Handle > ICFindPrefHandle( ICInstance        instance,
	                                  ConstStr255Param  key )
	{
		::ICAttr attr;
		return ICFindPrefHandle( instance, key, attr );
	}
	
	void RegisterInternetConfigErrors()
	{
		RegisterOSStatus< icPrefNotFoundErr        >();
		RegisterOSStatus< icPermErr                >();
		RegisterOSStatus< icPrefDataErr            >();
		RegisterOSStatus< icInternalErr            >();
		RegisterOSStatus< icTruncatedErr           >();
		RegisterOSStatus< icNoMoreWritersErr       >();
		RegisterOSStatus< icNothingToOverrideErr   >();
		RegisterOSStatus< icNoURLErr               >();
		RegisterOSStatus< icConfigNotFoundErr      >();
		RegisterOSStatus< icConfigInappropriateErr >();
		RegisterOSStatus< icProfileNotFoundErr     >();
		RegisterOSStatus< icTooManyProfilesErr     >();
	}
	
}

