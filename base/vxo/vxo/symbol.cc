/*
	symbol.cc
	---------
*/

#include "vxo/symbol.hh"

// Standard C
#include <string.h>


#pragma exceptions off


namespace vxo
{

Symbol::Symbol( const char* name )
{
	u.str.pointer  = name;
	u.str.length   = strlen( name );
	u.str.capacity = 0;
	
	set_subtype_byte( Box_symbolic );
	set_control_byte( Box_pointer  );
}

const char null[] = "null";

}
