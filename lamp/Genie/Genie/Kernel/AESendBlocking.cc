/*	=================
 *	AESendBlocking.cc
 *	=================
 */

// Nitrogen
#include "Nitrogen/AEDataModel.hh"
#include "Nitrogen/AEInteraction.hh"
#include "Nitrogen/OSStatus.hh"

// Nitrogen Extras / AEFramework
#include "AEFramework/AEFramework.h"

// Genie
#include "Genie/current_process.hh"
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	
	static OSStatus AESendBlocking( const AppleEvent* appleEventPtr, AppleEvent* replyPtr )
	{
		try
		{
			Mac::AppleEvent const& appleEvent = static_cast< const Mac::AppleEvent& >( *appleEventPtr );
			Mac::AppleEvent      & reply      = static_cast<       Mac::AppleEvent& >( *replyPtr      );
			
			(void) N::AESend( appleEvent,
			                  Mac::kAEQueueReply | Mac::kAECanInteract );
			
			// Now that we've sent the event, retrieve the return ID
			N::AEReturnID_32Bit returnID = N::AEGetAttributePtr< Mac::keyReturnIDAttr >( appleEvent );
			
			// Subscribe to AEFramework's queued reply delivery and wake-up service
			N::ExpectReply( returnID, &reply );
			
			// Sleep until the reply is delivered
			current_process().Stop();
		}
		catch ( const N::OSStatus& err )
		{
			return err;
		}
		
		return noErr;
	}
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( AESendBlocking );
	
	#pragma force_active reset
	
}

