// Nitrogen/AEInteraction.hh
// -------------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004-2007 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NITROGEN_AEINTERACTION_HH
#define NITROGEN_AEINTERACTION_HH

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __AEINTERACTION__
#include <AEInteraction.h>
#endif

// Nitrogen
#ifndef MAC_APPLEEVENTS_FUNCTIONS_AEDISPOSEDESC_HH
#include "Mac/AppleEvents/Functions/AEDisposeDesc.hh"
#endif
#ifndef MAC_APPLEEVENTS_TYPES_AESENDMODE_HH
#include "Mac/AppleEvents/Types/AESendMode.hh"
#endif
#ifndef MAC_APPLEEVENTS_TYPES_AESENDPRIORITY_HH
#include "Mac/AppleEvents/Types/AESendPriority.hh"
#endif

#ifndef NITROGEN_UPP_HH
#include "Nitrogen/UPP.hh"
#endif


namespace Nitrogen
{
	
	struct AEIdleUPP_Details : Basic_UPP_Details< ::AEIdleUPP,
	                                              ::AEIdleProcPtr,
	                                              ::NewAEIdleUPP,
	                                              ::DisposeAEIdleUPP,
	                                              ::InvokeAEIdleUPP >
	{};
	
	typedef UPP< AEIdleUPP_Details > AEIdleUPP;
	
	struct AEFilterUPP_Details : Basic_UPP_Details< ::AEFilterUPP,
	                                                ::AEFilterProcPtr,
	                                                ::NewAEFilterUPP,
	                                                ::DisposeAEFilterUPP,
	                                                ::InvokeAEFilterUPP >
	{};
	
	typedef UPP< AEFilterUPP_Details > AEFilterUPP;
	
	nucleus::owned< Mac::AppleEvent >
	//
	AESend( const Mac::AppleEvent&  appleEvent,
	        Mac::AESendMode         sendMode,
	        Mac::AESendPriority     sendPriority   = Mac::kAENormalPriority,
	        SInt32                  timeOutInTicks = kAEDefaultTimeout,
	        AEIdleUPP               idleProc       = NULL,
	        AEFilterUPP             filterProc     = NULL );
	
	inline nucleus::owned< AEIdleUPP > NewAEIdleUPP( AEIdleProcPtr p )
	{
			return NewUPP< AEIdleUPP >( p );
	}

	inline void DisposeAEIdleUPP( nucleus::owned< AEIdleUPP > )
	{
	}
	
	inline bool InvokeAEIdleUPP( EventRecord&  theEvent,
	                             SInt32&       sleepTime,
	                             RgnHandle&    mouseRgn,
	                             AEIdleUPP     userUPP )
	{
		return userUPP( &theEvent, &sleepTime, &mouseRgn );
	}

	inline nucleus::owned< AEFilterUPP > NewAEFilterUPP( AEFilterProcPtr p )
	{
			return NewUPP< AEFilterUPP >( p );
	}

	inline void DisposeAEFilterUPP( nucleus::owned< AEFilterUPP > )
	{
	}
	
	inline bool InvokeAEFilterUPP( EventRecord&               theEvent,
	                               long                       returnID,
	                               long                       transactionID,
	                               const Mac::AEAddressDesc&  sender,
	                               AEFilterUPP                userUPP )
	{
		return userUPP( &theEvent, returnID, transactionID, &sender );
	}
	
}

#endif
