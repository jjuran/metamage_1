/*	====================
 *	GenieHandlerReply.cc
 *	====================
 */

#include "Genie/ReplyHandler.hh"

// Mac OS
#include <AEObjects.h>
#include <AERegistry.h>

// Nitrogen
#include "Nitrogen/AEDataModel.h"
#include "Nitrogen/OSStatus.h"

// Nitrogen Extras / AEFramework
#include "AEFramework/AEFramework.h"

// Genie
#include "Genie/Process.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	
	void GenieHandlerReply::AppleEventHandler( const N::AppleEvent&   appleEvent,
	                                           N::AppleEvent&         /*reply*/,
	                                           GenieHandlerReply*     /*handler*/ )
	{
		N::ReceiveReply( appleEvent );
	}
	
	GenieHandlerReply::GenieHandlerReply()
	:
		NN::Owned< N::AEEventHandler >
		(
			N::AEInstallEventHandler< GenieHandlerReply*,
			                          AppleEventHandler >( N::kCoreEventClass,
			                                               N::kAEAnswer,
			                                               this )
		)
	{
	}
	
}

