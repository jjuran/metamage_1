/*
	gestalt.cc
	----------
*/

#include "mac_sys/gestalt.hh"

// mac-glue-utils
#include "mac_glue/Gestalt.hh"

// mac-sys-utils
#include "mac_sys/has/Gestalt.hh"


namespace mac {
namespace sys {

long gestalt( unsigned long selector, long default_value )
{
	if ( ! has_Gestalt() )
	{
		return default_value;
	}
	
	return mac::glue::gestalt_or( selector, default_value );
}

bool gestalt_defined( unsigned long selector )
{
	if ( ! has_Gestalt() )
	{
		return false;
	}
	
	return ! mac::glue::gestalt_err( selector );
}

}
}
