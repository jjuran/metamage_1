/*
	DAs.hh
	------
*/

#ifndef MACAPP_DAS_HH
#define MACAPP_DAS_HH

struct GrafPort;
struct OpaqueWindowPtr;

namespace mac {
namespace app {
	
	// One of these is defined; the other is never called.
	
	bool close_DA_window( GrafPort* window );
	bool close_DA_window( OpaqueWindowPtr* window );
	
	bool close_front_DA();
	
}
}

#endif
