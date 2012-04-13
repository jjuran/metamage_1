/*
	append_hex_encoded_byte.cc
	--------------------------
*/

#include "Genie/FS/append_hex_encoded_byte.hh"

// gear
#include "gear/hexidecimal.hh"

// plus
#include "plus/var_string.hh"


namespace Genie
{
	
	void append_hex_encoded_byte( plus::var_string& s, unsigned char c )
	{
		s += gear::encoded_hex_char( c >> 4 );
		s += gear::encoded_hex_char( c >> 0 );
	}
	
}

