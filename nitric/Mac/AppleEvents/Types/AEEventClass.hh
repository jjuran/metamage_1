/*
	Mac/AppleEvents/Types/AEEventClass.hh
	-------------------------------------
*/

#ifndef MAC_APPLEEVENTS_TYPES_AEEVENTCLASS_HH
#define MAC_APPLEEVENTS_TYPES_AEEVENTCLASS_HH

// Mac OS
#ifndef __AEDATAMODEL__
#include <AEDataModel.h>
#endif
#ifndef __AEREGISTRY__
#include <AERegistry.h>
#endif
#ifndef __APPLEEVENTS__
#include <AppleEvents.h>
#endif
#ifndef __ASREGISTRY__
#include <ASRegistry.h>
#endif
#ifndef __INTERNETCONFIG__
#include <InternetConfig.h>
#endif
#ifndef __OSA__
#include <OSA.h>
#endif

// nucleus
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
#endif


namespace Mac
{
	
	enum AEEventClass
	{
		// AppleEvents
		kCoreEventClass = ::kCoreEventClass,
		
		// AERegistry
		kAECoreSuite           = ::kAECoreSuite,
		kAEMiscStandards       = ::kAEMiscStandards,
		kAEQDSupplementalSuite = ::kAEQDSupplementalSuite,
		kAEQuickdrawSuite      = ::kAEQuickdrawSuite,
		kAERequiredSuite       = ::kAERequiredSuite,
		kAETableSuite          = ::kAETableSuite,
		kAETextSuite           = ::kAETextSuite,
		
		// ASRegistry
		kASAppleScriptSuite  = ::kASAppleScriptSuite,
		kASScriptEditorSuite = ::kASScriptEditorSuite,
		kASTypeNamesSuite    = ::kASTypeNamesSuite,
		
		// InternetConfig
		kInternetEventClass         = ::kInternetEventClass,
		kICEditPreferenceEventClass = ::kICEditPreferenceEventClass,
		
		// OSA
		kOSASuite = ::kOSASuite,
		
		kAEEventClass_Max = nucleus::enumeration_traits< ::AEEventClass >::max
	};
	
}

#endif

