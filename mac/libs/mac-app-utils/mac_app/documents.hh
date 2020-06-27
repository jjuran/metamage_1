/*
	documents.hh
	------------
*/

#ifndef MACAPP_DOCUMENTS_HH
#define MACAPP_DOCUMENTS_HH

// mac-config
#include "mac_config/apple-events.hh"

// mac-sys-utils
#include "mac_sys/gestalt.hh"

// mac-app-utils
#include "mac_app/event_handlers.hh"


namespace mac {
namespace app {
	
	void open_AppFiles_with( HFS_callback callback );
	
	inline
	void open_documents_with( HFS_callback callback )
	{
		// gestaltAppleEventsAttr = 'evnt'
		
		const bool apple_events_present =
			CONFIG_APPLE_EVENTS  &&
				(CONFIG_APPLE_EVENTS_GRANTED  ||  mac::sys::gestalt( 'evnt' ));
		
		if ( ! apple_events_present )
		{
			open_AppFiles_with( callback );
			return;
		}
		
		install_opendocs_handler( callback );
	}
	
	inline
	void open_documents_with( FSSpec_callback callback )
	{
		install_opendocs_handler( callback );
	}
	
	inline
	void open_documents_with( FSRef_callback callback )
	{
		install_opendocs_handler( callback );
	}
	
}
}

#endif
