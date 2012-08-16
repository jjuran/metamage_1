/*
	Mac/AppleEvents/Types/AESendMode.hh
	-----------------------------------
*/

#ifndef MAC_APPLEEVENTS_TYPES_AESENDMODE_HH
#define MAC_APPLEEVENTS_TYPES_AESENDMODE_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __AEDATAMODEL__
#include <AEDataModel.h>
#endif

// nucleus
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
#endif
#ifndef NUCLEUS_FLAGOPS_HH
#include "nucleus/flag_ops.hh"
#endif


namespace Mac
{
	
	enum AESendMode
	{
		kAENoReply               = ::kAENoReply,
		kAEQueueReply            = ::kAEQueueReply,
		kAEWaitReply             = ::kAEWaitReply,
		kAEDontReconnect         = ::kAEDontReconnect,
		kAEWantReceipt           = ::kAEWantReceipt,
		kAENeverInteract         = ::kAENeverInteract,
		kAECanInteract           = ::kAECanInteract,
		kAEAlwaysInteract        = ::kAEAlwaysInteract,
		kAECanSwitchLayer        = ::kAECanSwitchLayer,
		kAEDontRecord            = ::kAEDontRecord,
		kAEDontExecute           = ::kAEDontExecute,
		kAEProcessNonReplyEvents = ::kAEProcessNonReplyEvents,
		
		kAESendMode_Max = nucleus::enumeration_traits< ::AESendMode >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( AESendMode )
	
}

#endif

