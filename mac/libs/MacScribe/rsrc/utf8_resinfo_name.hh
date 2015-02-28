/*
	utf8_resinfo_name.hh
	--------------------
*/

#ifndef MACSCRIBE_RSRC_UTF8RESINFONAME_HH
#define MACSCRIBE_RSRC_UTF8RESINFONAME_HH

// mac-types
#include "mac_types/ResInfo.hh"

// plus
#include "plus/string.hh"


namespace MacScribe
{
	
	plus::string get_utf8_name_from_ResInfo( const mac::types::ResInfo& info );
	
}

#endif
