/*
	append_hex_encoded_byte.hh
	--------------------------
*/

#ifndef GENIE_FS_APPENDHEXENCODEDBYTE_HH
#define GENIE_FS_APPENDHEXENCODEDBYTE_HH

// plus
#include "plus/var_string_fwd.hh"


namespace Genie
{
	
	void append_hex_encoded_byte( plus::var_string& s, unsigned char c );
	
}

#endif

