/*
	GetStringResource.cc
	--------------------
*/

#include "Genie/Utilities/GetStringResource.hh"

// mac-rsrc-utils
#include "mac_rsrc/get_string_from_resource.hh"


namespace Genie
{
	
	plus::string GetStringResource( short id )
	{
		using mac::rsrc::get_STR_resource;
		
		unsigned char str[ 255 ];
		
		if ( get_STR_resource( str, id ) )
		{
			return plus::string( str );
		}
		
		return plus::string::null;
	}
	
}
