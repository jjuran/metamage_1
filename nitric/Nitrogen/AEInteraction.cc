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


namespace Nitrogen
{
	
	using Mac::ThrowOSStatus;
	
	
	NUCLEUS_REQUIRE_ERRORS_STATIC( AppleEventManager )
	
	nucleus::owned< Mac::AppleEvent >
	//
	AESend( const Mac::AppleEvent&  appleEvent,
	        Mac::AESendMode         sendMode,
	        Mac::AESendPriority     sendPriority,
	        SInt32                  timeOutInTicks,
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
	
}
