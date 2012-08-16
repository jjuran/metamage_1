/*
	Mac/AppleEvents/Types/AEKeyForm.hh
	----------------------------------
*/

#ifndef MAC_APPLEEVENTS_TYPES_AEKEYFORM_HH
#define MAC_APPLEEVENTS_TYPES_AEKEYFORM_HH

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __AEOBJECTS__
#include <AEObjects.h>
#endif
#ifndef __AEREGISTRY__
#include <AERegistry.h>
#endif
#ifndef __ASREGISTRY__
#include <ASRegistry.h>
#endif


namespace Mac
{
	
	enum AEKeyForm
	{
		// AEObjects
		formAbsolutePosition = ::formAbsolutePosition,
		formRelativePosition = ::formRelativePosition,
		formTest             = ::formTest,
		formRange            = ::formRange,
		formPropertyID       = ::formPropertyID,
		formName             = ::formName,
		
		// AERegistry
		formUniqueID = ::formUniqueID,
		
		// ASRegistry
		formUserPropertyID = ::formUserPropertyID,
		
		kAEKeyForm_Max = 0xFFFFFFFFu
	};
	
}

#endif

