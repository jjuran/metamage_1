/*
	FinderSync.hh
	-------------
*/

#ifndef GENIE_UTILITIES_FINDERSYNC_HH
#define GENIE_UTILITIES_FINDERSYNC_HH

struct FSSpec;


namespace Genie
{
	
	void send_Finder_sync_event( const FSSpec& spec );
	
}

#endif
