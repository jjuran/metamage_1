/*	=================
 *	InternetConfig.cp
 *	=================
 */

#ifndef NITROGEN_INTERNETCONFIG_H
#include "Nitrogen/InternetConfig.h"
#endif

#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif

namespace Nitrogen
{
	
	Owned< ICInstance > ICStart( OSType signature )
	{
		OnlyOnce< RegisterInternetConfigErrors >();
		
		ICInstance result;
		
		ThrowOSStatus( ::ICStart( &result, signature ) );
		
		return Owned< ICInstance >::Seize( result );
	}
	
	void ICStop( Owned< ICInstance > inst )
	{
		OnlyOnce< RegisterInternetConfigErrors >();
		
		ThrowOSStatus( ::ICStop( inst.Release() ) );
	}
	
	Owned< Handle > ICFindPrefHandle( ICInstance        instance,
	                                  ConstStr255Param  key,
	                                  ICAttr&           attr )
	{
		OnlyOnce< RegisterInternetConfigErrors >();
		
		Owned< Handle > prefh = NewHandle( 0 );
		ThrowOSStatus( ::ICFindPrefHandle( instance, key, &attr, prefh.Get() ) );
		return prefh;
	}
	
	Owned< Handle > ICFindPrefHandle( ICInstance        instance,
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

