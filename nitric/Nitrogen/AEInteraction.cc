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

// Nitrogen
#include "Nitrogen/AEDataModel.hh"
#include "Nitrogen/Processes.hh"


namespace Nitrogen
{
	
	NUCLEUS_REQUIRE_ERRORS_STATIC( AppleEventManager )
	
	nucleus::owned< Mac::AppleEvent >
	//
	AESend( const Mac::AppleEvent&  appleEvent,
	        Mac::AESendMode         sendMode,
	        Mac::AESendPriority     sendPriority,
	        long                    timeOutInTicks,
	        AEIdleUPP               idleProc,
	        AEFilterUPP             filterProc )
	{
		Mac::AppleEvent reply;
		
		ThrowOSStatus( ::AESend( &appleEvent,
		                         &reply,
		                         sendMode,
		                         sendPriority,
		                         timeOutInTicks,
		                         idleProc,
		                         filterProc ) );
		
		return nucleus::owned< Mac::AppleEvent >::seize( reply );
	}
	
	nucleus::owned< Mac::AppleEvent > AESend( Mac::AEEventClass eventClass, Mac::AEEventID eventID )
	{
		return AESend( AECreateAppleEvent( eventClass,
				                           eventID,
				                           AECreateDesc< Mac::typeProcessSerialNumber >( CurrentProcess() ) ),
				       Mac::kAEWaitReply );
	}
	
	void AEProcessAppleEvent( const EventRecord& event )
	{
		ThrowOSStatus( ::AEProcessAppleEvent( &event ) );
	}
	
}

