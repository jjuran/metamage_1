/*
	scoped_port.hh
	--------------
*/

#ifndef SCOPEDPORT_HH
#define SCOPEDPORT_HH

// ams-common
#include "QDGlobals.hh"


struct GrafPort;


class scoped_port
{
	private:
		typedef GrafPort* GrafPtr;
		
		GrafPtr its_saved_port;
		
		// non-copyable
		scoped_port           ( const scoped_port& );
		scoped_port& operator=( const scoped_port& );
	
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
