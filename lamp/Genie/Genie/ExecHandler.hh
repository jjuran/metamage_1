/*	===================
 *	GenieExecHandler.hh
 *	===================
 */

#pragma once

// Nitrogen
#include "Nitrogen/AppleEvents.h"
#include "Nitrogen/Files.h"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	class GenieExecHandler : public N::Owned< N::AEEventHandler >
	{
		public:
			static void AppleEventHandler( const AppleEvent& appleEvent, AppleEvent& reply, GenieExecHandler* handler );
			
			GenieExecHandler();
			
			void HandleAppleEvent( const AppleEvent& appleEvent, AppleEvent& outReply );
			
			int ExecArgList( const std::vector< std::string >& argVec );
			int ExecString( const std::string& cmd, const N::FSDirSpec& cwd );
	};
	
}

