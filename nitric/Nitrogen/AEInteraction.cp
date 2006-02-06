// AEInteraction.cp

#ifndef NITROGEN_AEINTERACTION_H
#include "Nitrogen/AEInteraction.h"
#endif

namespace Nitrogen
{
	
	Nucleus::Owned< AppleEvent > AESend( const AppleEvent&  appleEvent,
	                            AESendMode         sendMode,
	                            AESendPriority     sendPriority,
	                            long               timeOutInTicks,
	                            AEIdleUPP          idleProc,
	                            AEFilterUPP        filterProc )
	{
		Nucleus::OnlyOnce< RegisterAppleEventManagerErrors >();
		
		AEDesc reply;
		
		ThrowOSStatus( ::AESend( &appleEvent,
		                         &reply,
		                         sendMode,
		                         sendPriority,
		                         timeOutInTicks,
		                         idleProc,
		                         filterProc ) );
		
		return Nucleus::Owned< AppleEvent >::Seize( reply );
	}
	
	void AEProcessAppleEvent( const EventRecord& event )
	{
		Nucleus::OnlyOnce< RegisterAppleEventManagerErrors >();
		
		ThrowOSStatus( ::AEProcessAppleEvent( &event ) );
	}
	
}

