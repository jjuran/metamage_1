/*
	event_handlers.hh
	-----------------
*/

#ifndef MACAPP_EVENTHANDLERS_HH
#define MACAPP_EVENTHANDLERS_HH

struct FSRef;
struct FSSpec;

namespace mac {
namespace app {
	
	typedef long (*FSSpec_callback)( const FSSpec& file );
	typedef long (*FSRef_callback )( const FSRef&  file );
	
	void install_basic_event_handlers();
	
	void install_opendocs_handler( FSSpec_callback );
	void install_opendocs_handler( FSRef_callback  );
	
}
}

#endif
