/*	============
 *	GenieMain.cc
 *	============
 */

// Nitrogen
#include "Nitrogen/AppleEvents.hh"

// Pedestal
#include "Pedestal/Application.hh"
#include "Pedestal/Commands.hh"

// Nitrogen Extras / AEFramework
#include "AEFramework/AEFramework.h"

// Genie
#include "Genie/ProcessList.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace Ped = Pedestal;
	
	
	struct Reply_AppleEvent
	{
		static void Handler( N::AppleEvent const&  event,
		                     N::AppleEvent&        reply )
		{
			N::ReceiveReply( event );
		}
		
		static void Install_Handler()
		{
			N::AEInstallEventHandler< Handler >( N::kCoreEventClass,
			                                     N::kAEAnswer ).release();
		}
	};
	
	
	class App : public Ped::Application
	{
		public:
			App();
			~App()  { kill_all_processes(); }
	};
	
	
	static bool About( Ped::CommandCode )
	{
		spawn_process( "/sbin/about" );
		
		return true;
	}
	
	static bool NewDocument( Ped::CommandCode )
	{
		spawn_process( "/bin/jgetty" );
		
		return true;
	}
	
	App::App()
	{
		SetCommandHandler( Ped::kCmdAbout, &About       );
		SetCommandHandler( Ped::kCmdNew,   &NewDocument );
		
		Reply_AppleEvent::Install_Handler();
	}
	
}


//	main
//	----

int main()
{
	Genie::App app;
	
	return app.Run();
}

