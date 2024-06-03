/*
	App.cc
	------
*/

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#ifndef MAC_OS_X_VERSION_10_5
#include <ApplicationServices/ApplicationServices.h>
#endif
#endif

// Mac OS
#ifndef __APPLEEVENTS__
#include <AppleEvents.h>
#endif

// Annex
#ifndef ANNEX_MACTYPES_H
#include "Annex/MacTypes.h"
#endif

// mac-config
#include "mac_config/apple-events.hh"
#include "mac_config/upp-macros.hh"

// cthread-either
#include "cthread-either.hh"

// AEWakeup
#include "AEWakeup/AEWakeup.hh"

// plus
#include "plus/string.hh"

// Pedestal
#include "Pedestal/AboutBox.hh"
#include "Pedestal/Application.hh"
#include "Pedestal/Commands.hh"

// relix-kernel
#include "relix/api/os_thread_api.hh"
#include "relix/task/scheduler.hh"
#include "relix/task/thread.hh"

// Genie
#include "Genie/notify.hh"
#include "Genie/ProcessList.hh"
#include "Genie/mnt/listener.hh"


namespace Ped = Pedestal;


namespace Genie
{
	
	static bool in_user_thread;
	
	static
	cthread::thread_id os_thread_scheduler()
	{
		using namespace relix;
		
		in_user_thread = ! in_user_thread;
		
		if ( ! in_user_thread )
		{
			return cthread::thread_id();
		}
		
		if ( queue_element* next = schedule_next_task() )
		{
			thread* next_thread = thread::from_queue_element( next );
			
			return next_thread->get_os_thread();
		}
		
		return cthread::thread_id();
	}
	
	static
	pascal OSErr Handle_Reply_event( AppleEvent const*  event,
	                                 AppleEvent*        reply,
	                                 SRefCon )
	{
		return AEWakeup::Deliver( *event );
	}
	
	
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
	
	static inline
	void install_handlers()
	{
		const bool apple_events_present =
			CONFIG_APPLE_EVENTS  &&
				(CONFIG_APPLE_EVENTS_GRANTED  ||
					Ped::apple_events_present);
		
	#ifndef __RELIX__
		
		(void) signal( SIGPIPE, SIG_IGN );
		
		install_empty_sighup_handler();
		
		start_gui_service();
		
	#endif
		
		Ped::gThreadYield_Hook  = &relix::os_thread_yield;
		Ped::gActivelyBusy_Hook = &relix::is_active;
		Ped::gReadyToExit_Hook  = &is_ready_to_exit;
		
		cthread::set_thread_scheduler( &os_thread_scheduler );
		
		SetCommandHandler( Ped::kCmdAbout, &About       );
		SetCommandHandler( Ped::kCmdNew,   &NewDocument );
		
		if ( apple_events_present )
		{
			DEFINE_UPP( AEEventHandler, Handle_Reply_event );
			
			AEInstallEventHandler( kCoreEventClass,
			                       kAEAnswer,
			                       UPP_ARG( Handle_Reply_event ),
			                       0,
			                       false );
		}
		
		spawn_process( "/etc/startup" );
	}
	
}


//	main
//	----

int main()
{
	Ped::Application app;
	
	Genie::install_handlers();
	
	return app.Run();
}
