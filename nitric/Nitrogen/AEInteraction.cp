// AEInteraction.cp

#ifndef NITROGEN_AEINTERACTION_H
#include "Nitrogen/AEInteraction.h"
#endif

namespace Nitrogen
{
	
	static AppleEventManagerErrorsRegistrationDependency gAppleEventManagerErrorsRegistrationDependency;
	
	Nucleus::Owned< AppleEvent > AESend( const AppleEvent&  appleEvent,
	                                     AESendMode         sendMode,
	                                     AESendPriority     sendPriority,
	                                     long               timeOutInTicks,
	                                     AEIdleUPP          idleProc,
	                                     AEFilterUPP        filterProc )
	{
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
		ThrowOSStatus( ::AEProcessAppleEvent( &event ) );
	}
	
}

