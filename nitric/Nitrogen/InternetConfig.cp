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


#include "Nitrogen/InternetConfig.h"

// Nitrogen
#include "Nitrogen/OSStatus.h"


namespace Nitrogen
{
	
	// does nothing, but guarantees construction of theRegistration
	NUCLEUS_DEFINE_ERRORS_DEPENDENCY( InternetConfig )
	
	
	static void RegisterInternetConfigErrors();
	
	
#pragma force_active on
	
	class InternetConfigErrorsRegistration
	{
		public:
			InternetConfigErrorsRegistration()  { RegisterInternetConfigErrors(); }
	};
	
	static InternetConfigErrorsRegistration theRegistration;
	
#pragma force_active reset
	
	
	Nucleus::Owned< ICInstance > ICStart( OSType signature )
	{
		::ICInstance instance;
		
		ThrowOSStatus( ::ICStart( &instance, signature ) );
		
		return Nucleus::Owned< ICInstance >::Seize( ICInstance( instance ) );
	}
	
	void ICStop( Nucleus::Owned< ICInstance >& instance )
	{
		ThrowOSStatus( ::ICStop( instance.Get() ) );
		
		instance.Release();
	}
	
	
	static Nucleus::Owned< Handle > ICFindPrefHandle( ICInstance        instance,
	                                                  ConstStr255Param  key,
	                                                  ::ICAttr&         attr )
	{
		Nucleus::Owned< Handle > prefh = NewHandle( 0 );
		
		ThrowOSStatus( ::ICFindPrefHandle( instance, key, &attr, prefh.Get() ) );
		
		return prefh;
	}
	
	Nucleus::Owned< Handle > ICFindPrefHandle( ICInstance        instance,
	                                           ConstStr255Param  key,
	                                           ICAttr&           attr )
	{
		::ICAttr tempAttr;
		
		Nucleus::Owned< Handle > prefh = ICFindPrefHandle( instance, key, tempAttr );
		
		attr = ICAttr( tempAttr );
		
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

