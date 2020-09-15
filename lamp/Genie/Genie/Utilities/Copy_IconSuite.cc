/*
	Copy_IconSuite.cc
	-----------------
*/

#include "Genie/Utilities/Copy_IconSuite.hh"

// mac-config
#include "mac_config/upp-macros.hh"


namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	
	
	#pragma exceptions off
	
	static pascal OSErr CopyIconToSuite( ::ResType type, ::Handle* icon, void* userData )
	{
		::IconSuiteRef newSuite = (::IconSuiteRef) userData;
		
		::Handle copy = *icon;
		
		if ( copy == NULL )
		{
			return noErr;
		}
		
		OSErr err = ::HandToHand( &copy );
		
		if ( err != noErr )
		{
			return err;
		}
		
		err = ::AddIconToSuite( copy, newSuite, type );
		
		if ( err != noErr )
		{
			::DisposeHandle( copy );
		}
		
		return err;
	}
	
	#pragma exceptions reset
	
	
	n::owned< N::IconSuiteRef > Copy_IconSuite( N::IconSuiteRef iconSuite )
	{
		DEFINE_UPP( IconAction, CopyIconToSuite )
		
		n::owned< N::IconSuiteRef > copy = N::NewIconSuite();
		
		Mac::ThrowOSStatus( ::ForEachIconDo( iconSuite,
		                                     kSelectorAllAvailableData,
		                                     UPP_ARG( CopyIconToSuite ),
		                                     copy.get().Get() ) );
		
		return copy;
	}
	
}
