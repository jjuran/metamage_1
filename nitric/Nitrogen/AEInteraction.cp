// Nitrogen/AEInteraction.cp
// -------------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004-2007 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


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
	
	Nucleus::Owned< AppleEvent > AESend( AEEventClass eventClass, AEEventID eventID )
	{
		return AESend( AECreateAppleEvent( eventClass,
				                           eventID,
				                           AECreateDesc< typeProcessSerialNumber >( CurrentProcess() ) ),
				       AESendMode( kAEWaitReply ) );
	}
	
	void AEProcessAppleEvent( const EventRecord& event )
	{
		ThrowOSStatus( ::AEProcessAppleEvent( &event ) );
	}
	
}

