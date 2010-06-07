// Nitrogen/InternetConfig.cc
// --------------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004-2006 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#include "Nitrogen/InternetConfig.hh"

// Nitrogen
#include "Nitrogen/OSStatus.hh"


namespace Nitrogen
{
	
	// does nothing, but guarantees construction of theRegistration
	NUCLEUS_DEFINE_ERRORS_DEPENDENCY( InternetConfig )
	
	
	static void RegisterInternetConfigErrors();
	
	
#if NUCLEUS_RICH_ERRORCODES
#pragma force_active on
	
	class InternetConfigErrorsRegistration
	{
		public:
			InternetConfigErrorsRegistration()  { RegisterInternetConfigErrors(); }
	};
	
	static InternetConfigErrorsRegistration theRegistration;
	
#pragma force_active reset
#endif
	
	
	nucleus::owned< ICInstance > ICStart( Mac::OSType signature )
	{
		::ICInstance instance;
		
		ThrowOSStatus( ::ICStart( &instance, signature ) );
		
		return nucleus::owned< ICInstance >::seize( ICInstance( instance ) );
	}
	
	void ICStop( nucleus::owned< ICInstance >& instance )
	{
		ThrowOSStatus( ::ICStop( instance.get() ) );
		
		instance.release();
	}
	
	
	static nucleus::owned< Handle > ICFindPrefHandle( ICInstance        instance,
	                                                  ConstStr255Param  key,
	                                                  ::ICAttr&         attr )
	{
		nucleus::owned< Handle > prefh = NewHandle( 0 );
		
		ThrowOSStatus( ::ICFindPrefHandle( instance, key, &attr, prefh.get() ) );
		
		return prefh;
	}
	
	nucleus::owned< Handle > ICFindPrefHandle( ICInstance        instance,
	                                           ConstStr255Param  key,
	                                           ICAttr&           attr )
	{
		::ICAttr tempAttr;
		
		nucleus::owned< Handle > prefh = ICFindPrefHandle( instance, key, tempAttr );
		
		attr = ICAttr( tempAttr );
		
		return prefh;
	}
	
	nucleus::owned< Handle > ICFindPrefHandle( ICInstance        instance,
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

