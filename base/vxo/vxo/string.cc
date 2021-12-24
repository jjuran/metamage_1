/*
	string.cc
	---------
*/

#include "vxo/string.hh"

// Standard C
#include <string.h>


#pragma exceptions off


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

}
