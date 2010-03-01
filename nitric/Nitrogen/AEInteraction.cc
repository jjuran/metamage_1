// Nitrogen/AEInteraction.cc
// -------------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004-2007 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#include "Nitrogen/AEInteraction.hh"


namespace Nitrogen
{
	
	NUCLEUS_REQUIRE_ERRORS_STATIC( AppleEventManager )
	
	nucleus::owned< AppleEvent > AESend( const AppleEvent&  appleEvent,
	                                     AESendMode         sendMode,
	                                     AESendPriority     sendPriority,
	                                     long               timeOutInTicks,
	                                     AEIdleUPP          idleProc,
	                                     AEFilterUPP        filterProc )
	{
		AppleEvent reply;
		
		ThrowOSStatus( ::AESend( &appleEvent,
		                         &reply,
		                         sendMode,
		                         sendPriority,
		                         timeOutInTicks,
		                         idleProc,
		                         filterProc ) );
		
		return nucleus::owned< AppleEvent >::seize( reply );
	}
	
	nucleus::owned< AppleEvent > AESend( AEEventClass eventClass, AEEventID eventID )
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

