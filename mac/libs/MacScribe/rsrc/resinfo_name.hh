/*
	resinfo_name.hh
	---------------
*/

#ifndef MACSCRIBE_RSRC_RESINFONAME_HH
#define MACSCRIBE_RSRC_RESINFONAME_HH

// mac-types
#include "mac_types/ResInfo.hh"

// plus
#include "plus/string.hh"


namespace MacScribe
{
	
	plus::string get_name_from_ResInfo( const mac::types::ResInfo& info );
	
}

#endif
