/*	=====================
 *	Genie/HandlerReply.hh
 *	=====================
 */

#ifndef GENIE_REPLYHANDLER_HH
#define GENIE_REPLYHANDLER_HH

// Nitrogen
#include "Nitrogen/AppleEvents.hh"


namespace Genie
{
	
	class GenieHandlerReply : public nucleus::owned< Nitrogen::AEEventHandler >
	{
		public:
			static void AppleEventHandler( const Nitrogen::AppleEvent& appleEvent, Nitrogen::AppleEvent& reply, GenieHandlerReply* handler );
			
			GenieHandlerReply();
	};
	
}

#endif

