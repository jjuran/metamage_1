/*
	event_handlers.hh
	-----------------
*/

#ifndef MACAPP_EVENTHANDLERS_HH
#define MACAPP_EVENTHANDLERS_HH

struct FSRef;
struct FSSpec;

typedef unsigned char Byte;

namespace mac {
namespace app {
	
	typedef long (*HFS_callback)( short vRefNum, long dirID, const Byte* name );
	
	typedef long (*FSSpec_callback)( const FSSpec& file );
	typedef long (*FSRef_callback )( const FSRef&  file );
	
	void install_basic_event_handlers();
	
	void install_opendocs_handler( HFS_callback    );
	void install_opendocs_handler( FSSpec_callback );
	void install_opendocs_handler( FSRef_callback  );
	
}
}

#endif
