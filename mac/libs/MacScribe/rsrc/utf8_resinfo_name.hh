/*
	utf8_resinfo_name.hh
	--------------------
*/

#ifndef MACSCRIBE_RSRC_UTF8RESINFONAME_HH
#define MACSCRIBE_RSRC_UTF8RESINFONAME_HH

// plus
#include "plus/string.hh"


namespace Nitrogen
{
	
	struct GetResInfo_Result;
	
}

namespace MacScribe
{
	
	plus::string get_utf8_name_from_ResInfo( const Nitrogen::GetResInfo_Result& info );
	
}

#endif

