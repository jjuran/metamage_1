/*
	App.cc
	------
*/

// mac-config
#include "mac_config/apple-events.hh"

// Nitrogen
#include "Nitrogen/AppleEvents.hh"

// Pedestal
#include "Pedestal/AboutBox.hh"
#include "Pedestal/Application.hh"
#include "Pedestal/Commands.hh"

// Nitrogen Extras / AEFramework
#include "AEFramework/AEFramework.h"

// relix-kernel
#include "relix/api/os_thread_api.hh"
#include "relix/task/scheduler.hh"

// Genie
#include "Genie/notify.hh"
#include "Genie/ProcessList.hh"
#include "Genie/mnt/listener.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace Ped = Pedestal;
	
	
	struct Reply_AppleEvent
	{
		static void Handler( Mac::AppleEvent const&  event,
		                     Mac::AppleEvent&        reply )
		{
			N::ReceiveReply( event );
		}
		
		static void Install_Handler()
		{
			N::AEInstallEventHandler< Handler >( Mac::kCoreEventClass,
			                                     Mac::kAEAnswer ).release();
		}
	};
	
	
	class App : public Ped::Application
	{
		public:
			App();
	};
	
	
	static bool About( Ped::CommandCode )
	{
		Ped::ShowAboutBox();
		
		return true;
	}
	
	static bool NewDocument( Ped::CommandCode )
	{
		spawn_process( "/bin/teletype" );
		
		return true;
	}
	
	App::App()
	{
		const bool apple_events_present =
			CONFIG_APPLE_EVENTS  &&
				(CONFIG_APPLE_EVENTS_GRANTED  ||
					Ped::apple_events_present);
		
	#ifndef __RELIX__
		
		install_empty_sighup_handler();
		
		start_gui_service();
		
	#endif
		
		Ped::gThreadYield_Hook  = &relix::os_thread_yield;
		Ped::gActivelyBusy_Hook = &relix::is_active;
		Ped::gReadyToExit_Hook  = &is_ready_to_exit;
		
		SetCommandHandler( Ped::kCmdAbout, &About       );
		SetCommandHandler( Ped::kCmdNew,   &NewDocument );
		
		if ( apple_events_present )
		{
			Reply_AppleEvent::Install_Handler();
		}
		
		spawn_process( "/etc/startup" );
	}
	
}


//	main
//	----

int main()
{
	Genie::App app;
	
	return app.Run();
}
