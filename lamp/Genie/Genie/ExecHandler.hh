/*	====================
 *	Genie/ExecHandler.hh
 *	====================
 */

#ifndef GENIE_EXECHANDLER_HH
#define GENIE_EXECHANDLER_HH

// Nitrogen
#include "Nitrogen/AppleEvents.hh"
#include "Nitrogen/Files.hh"


namespace Genie
{
	
	class GenieExecHandler : public nucleus::owned< Nitrogen::AEEventHandler >
	{
		public:
			static void AppleEventHandler( const Nitrogen::AppleEvent& appleEvent, Nitrogen::AppleEvent& reply, GenieExecHandler* handler );
			
			GenieExecHandler();
			
			void HandleAppleEvent( const Nitrogen::AppleEvent& appleEvent, Nitrogen::AppleEvent& outReply );
			
			int ExecArgList( const std::vector< std::string >& argVec );
			int ExecString( const std::string& cmd, const Nitrogen::FSDirSpec& cwd );
	};
	
}

#endif

