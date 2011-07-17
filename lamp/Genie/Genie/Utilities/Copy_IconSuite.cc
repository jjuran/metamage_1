/*
	Copy_IconSuite.cc
	-----------------
*/

#include "Genie/Utilities/Copy_IconSuite.hh"


namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	
	
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
	
	static ::IconActionUPP gCopyIconToSuiteUPP = ::NewIconActionUPP( &CopyIconToSuite );
	
	n::owned< N::IconSuiteRef > Copy_IconSuite( N::IconSuiteRef iconSuite )
	{
		n::owned< N::IconSuiteRef > copy = N::NewIconSuite();
		
		Mac::ThrowOSStatus( ::ForEachIconDo( iconSuite,
		                                     kSelectorAllAvailableData,
		                                     gCopyIconToSuiteUPP,
		                                     copy.get().Get() ) );
		
		return copy;
	}
	
}

