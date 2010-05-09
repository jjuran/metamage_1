/*
	append_hex_encoded_byte.cc
	--------------------------
*/

#include "Genie/FS/append_hex_encoded_byte.hh"

// iota
#include "iota/hexidecimal.hh"

// plus
#include "plus/var_string.hh"


namespace Genie
{
	
	void append_hex_encoded_byte( plus::var_string& s, unsigned char c )
	{
		const plus::string::size_type size = s.size();
		
		s.resize( size + 2 * sizeof c );
		
		s[ size     ] = iota::encoded_hex_char( c >> 4 );
		s[ size + 1 ] = iota::encoded_hex_char( c >> 0 );
	}
	
}

