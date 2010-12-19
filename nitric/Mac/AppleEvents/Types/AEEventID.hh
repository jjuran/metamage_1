/*
	Mac/AppleEvents/Types/AEEventID.hh
	----------------------------------
*/

#ifndef MAC_APPLEEVENTS_TYPES_AEEVENTID_HH
#define MAC_APPLEEVENTS_TYPES_AEEVENTID_HH

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

// nucleus
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
#endif


namespace Mac
{
	
	enum AEEventID
	{
		// AppleEvents
		kAEOpenApplication = ::kAEOpenApplication,
		kAEOpenDocuments   = ::kAEOpenDocuments,
		kAEPrintDocuments  = ::kAEPrintDocuments,
		kAEQuitApplication = ::kAEQuitApplication,
		kAEAnswer          = ::kAEAnswer,
		kAEApplicationDied = ::kAEApplicationDied,
		kAEShowPreferences = ::kAEShowPreferences,
		
		kAEStartRecording       = ::kAEStartRecording,
		kAEStopRecording        = ::kAEStopRecording,
		kAENotifyStartRecording = ::kAENotifyStartRecording,
		kAENotifyStopRecording  = ::kAENotifyStopRecording,
		kAENotifyRecording      = ::kAENotifyRecording,
		
		// AERegistry
		kAEClone             = ::kAEClone,
		kAEClose             = ::kAEClose,
		kAECopy              = ::kAECopy,
		kAECountElements     = ::kAECountElements,
		kAECreateElement     = ::kAECreateElement,
		kAECreatePublisher   = ::kAECreatePublisher,
		kAECut               = ::kAECut,
		kAEDelete            = ::kAEDelete,
		kAEDoObjectsExist    = ::kAEDoObjectsExist,
		kAEDoScript          = ::kAEDoScript,
		kAEGetClassInfo      = ::kAEGetClassInfo,
		kAEGetData           = ::kAEGetData,
		kAEGetDataSize       = ::kAEGetDataSize,
		kAEGetEventInfo      = ::kAEGetEventInfo,
		kAEGetSuiteInfo      = ::kAEGetSuiteInfo,
		kAEMove              = ::kAEMove,
		kAEOpen              = ::kAEOpen,
		kAEPaste             = ::kAEPaste,
		kAEPrint             = ::kAEPrint,
		kAEReopenApplication = ::kAEReopenApplication,
		kAESave              = ::kAESave,
		kAESelect            = ::kAESelect,
		kAESetData           = ::kAESetData,
		
		// ASRegistry
		kASCommentEvent    = ::kASCommentEvent,
		kASLaunchEvent     = ::kASLaunchEvent,
		kASSubroutineEvent = ::kASSubroutineEvent,
		
		kASEqual              = ::kASEqual,
		kASNotEqual           = ::kASNotEqual,
		kASGreaterThan        = ::kASGreaterThan,
		kASGreaterThanOrEqual = ::kASGreaterThanOrEqual,
		kASLessThan           = ::kASLessThan,
		kASLessThanOrEqual    = ::kASLessThanOrEqual,
		kASStartsWith         = ::kASStartsWith,
		kASEndsWith           = ::kASEndsWith,
		kASContains           = ::kASContains,
		
		kGetAETE = ::kGetAETE,
		kGetAEUT = ::kGetAEUT,
		
		// InternetConfig
		kAEGetURL              = ::kAEGetURL,
		kAEFetchURL            = ::kAEFetchURL,
		kICEditPreferenceEvent = ::kICEditPreferenceEvent,
		
		kAEEventID_Max = nucleus::enumeration_traits< ::AEEventID >::max
	};
	
}

#endif

