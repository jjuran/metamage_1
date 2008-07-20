/*	=====================
 *	Genie/HandlerReply.hh
 *	=====================
 */

#ifndef GENIE_REPLYHANDLER_HH
#define GENIE_REPLYHANDLER_HH

// Nitrogen
#include "Nitrogen/AppleEvents.h"


namespace Genie
{
	
	class GenieHandlerReply : public Nucleus::Owned< Nitrogen::AEEventHandler >
	{
		public:
			static void AppleEventHandler( const AppleEvent& appleEvent, AppleEvent& reply, GenieHandlerReply* handler );
			
			GenieHandlerReply();
	};
	
}

#endif

