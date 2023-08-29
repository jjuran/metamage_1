/*
	copy_IconSuite.cc
	-----------------
*/

#if ! __LP64__

#include "mac_icon/copy_IconSuite.hh"

// Mac OS
#ifndef __ICONS__
#include <Icons.h>
#endif

// mac-config
#include "mac_config/upp-macros.hh"


#pragma exceptions off


namespace mac  {
namespace icon {

static
pascal OSErr CopyIconToSuite( ResType type, Handle* icon, void* userData )
{
	IconSuiteRef newSuite = (IconSuiteRef) userData;
	
	Handle copy = *icon;
	
	if ( copy == NULL )
	{
		return noErr;
	}
	
	OSErr err = HandToHand( &copy );
	
	if ( err != noErr )
	{
		return err;
	}
	
	err = AddIconToSuite( copy, newSuite, type );
	
	if ( err != noErr )
	{
		DisposeHandle( copy );
	}
	
	return err;
}

IconSuiteRef copy_IconSuite( IconSuiteRef iconSuite )
{
	OSErr err;
	
	DEFINE_UPP( IconAction, CopyIconToSuite )
	
	IconSuiteRef copy;
	
	if ( (err = NewIconSuite( &copy )) )
	{
		return NULL;
	}
	
	err = ForEachIconDo( iconSuite,
	                     kSelectorAllAvailableData,
	                     UPP_ARG( CopyIconToSuite ),
	                     copy );
	
	if ( err )
	{
		DisposeIconSuite( copy, true );
		
		return NULL;
	}
	
	return copy;
}

}
}

#else  // #if ! __LP64__

int dummy;

#endif
