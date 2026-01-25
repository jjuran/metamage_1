/*
	scoped_port.hh
	--------------
*/

#ifndef SCOPEDPORT_HH
#define SCOPEDPORT_HH

// iota
#include "iota/class.hh"

// ams-common
#include "QDGlobals.hh"


struct GrafPort;


class scoped_port
{
	NON_COPYABLE( scoped_port )
	NO_NEW_DELETE
	
	private:
		typedef GrafPort* GrafPtr;
		
		GrafPtr its_saved_port;
	
	public:
		scoped_port( GrafPtr port )
		{
			GrafPtr& thePort = *get_addrof_thePort();
			
			its_saved_port = thePort;
			thePort        = port;
		}
		
		~scoped_port()
		{
			*get_addrof_thePort() = its_saved_port;
		}
};

#endif
