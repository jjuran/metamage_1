/*
	string.cc
	---------
*/

#include "vxo/string.hh"

// Standard C
#include <string.h>

// plus
#include "plus/extent.hh"


namespace vxo
{

StaticString::StaticString( const char* s )
{
	u.str.pointer  = s;
	u.str.length   = strlen( s );
	u.str.capacity = 0;
	
	set_subtype_byte( Box_string  );
	set_control_byte( Box_pointer );
}

DynamicString::DynamicString( const plus::string& s )
{
	u.str = *(u_string*) &s;
	
	if ( has_extent() )
	{
		plus::extent_add_ref( u.str.pointer );
	}
	
	if ( has_pointer() )
	{
		set_subtype_byte( Box_string );
	}
}

}
