/*	====================
 *	GenieHandlerReply.hh
 *	====================
 */

#pragma once

// Nitrogen
#include "Nitrogen/AppleEvents.h"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	class GenieHandlerReply : public N::Owned< N::AEEventHandler >
	{
		public:
			static void AppleEventHandler( const AppleEvent& appleEvent, AppleEvent& reply, GenieHandlerReply* handler );
			
			GenieHandlerReply();
	};
	
}

