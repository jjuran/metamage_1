/*
	QDGlobals.cc
	------------
*/

#include "QDGlobals.hh"


#if CONFIG_DEBUGGING

GrafPtr get_thePort()
{
	GrafPtr port = *get_addrof_thePort();
	
	if ( ! port )
	{
		DebugStr( "\p" "*** qd.thePort is NULL! ***" );
	}
	
	return port;
}

#endif
