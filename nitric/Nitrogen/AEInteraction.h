// AEInteraction.h

#ifndef NITROGEN_AEINTERACTION_H
#define NITROGEN_AEINTERACTION_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __AEINTERACTION__
#include FRAMEWORK_HEADER(AE,AEInteraction.h)
#endif
#ifndef NITROGEN_AEDATAMODEL_H
#include "Nitrogen/AEDataModel.h"
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
	
	using ::AEIdleProcPtr;
	
	Nucleus::Owned< AppleEvent > AESend( const AppleEvent&  appleEvent,
	                                     AESendMode         sendMode,
	                                     AESendPriority     sendPriority   = kAENormalPriority,
	                                     long               timeOutInTicks = kAEDefaultTimeout,
	                                     AEIdleUPP          idleProc       = NULL,
	                                     AEFilterUPP        filterProc     = NULL
	);
	
	Nucleus::Owned< AppleEvent > AESend( AEEventClass eventClass, AEEventID eventID );
	
	void AEProcessAppleEvent( const EventRecord& event );
	
	inline Nucleus::Owned< AEIdleUPP > NewAEIdleUPP( AEIdleProcPtr p )
	{
			return NewUPP< AEIdleUPP >( p );
	}

	inline void DisposeAEIdleUPP( Nucleus::Owned< AEIdleUPP > )
	{
	}
	
	inline bool InvokeAEIdleUPP( EventRecord&  theEvent,
	                             long&         sleepTime,
	                             RgnHandle&    mouseRgn,
	                             AEIdleUPP     userUPP )
	{
		return userUPP( &theEvent, &sleepTime, &mouseRgn );
	}

	inline Nucleus::Owned< AEFilterUPP > NewAEFilterUPP( AEFilterProcPtr p )
	{
			return NewUPP< AEFilterUPP >( p );
	}

	inline void DisposeAEFilterUPP( Nucleus::Owned< AEFilterUPP > )
	{
	}
	
	inline bool InvokeAEFilterUPP( EventRecord&          theEvent,
	                               long                  returnID,
	                               long                  transactionID,
	                               const AEAddressDesc&  sender,
	                               AEFilterUPP           userUPP )
	{
		return userUPP( &theEvent, returnID, transactionID, &sender );
	}
	
}

#endif
