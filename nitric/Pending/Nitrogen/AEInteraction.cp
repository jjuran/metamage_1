// AEInteraction.cp

#ifndef NITROGEN_AEINTERACTION_H
#include "Nitrogen/AEInteraction.h"
#endif

namespace Nitrogen {
	
	Owned<AppleEvent> AESend(
		const AppleEvent& appleEvent, 
		AESendMode sendMode, 
		AESendPriority sendPriority, 
		long timeOutInTicks, 
		AEIdleUPP idleProc, 
		AEFilterUPP filterProc)
	{
		AEDesc reply;
		ThrowOSStatus(
			::AESend(
				&appleEvent, 
				&reply, 
				sendMode, 
				sendPriority, 
				timeOutInTicks, 
				idleProc, 
				filterProc
			)
		);
		return Owned<AppleEvent>::Seize( reply );
	}
	
	void AEProcessAppleEvent( const EventRecord& event )
	{
		ThrowOSStatus( ::AEProcessAppleEvent( &event ) );
	}
	
}

