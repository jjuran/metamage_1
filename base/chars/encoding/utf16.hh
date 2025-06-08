/*
	utf16.hh
	--------
*/

#ifndef ENCODING_UTF16_HH
#define ENCODING_UTF16_HH

// charsets
#include "charsets/unicode.hh"


namespace chars
{
	
	typedef unsigned short utf16_t;
	
	unichar_t get_next_code_point_from_utf16( const utf16_t*&  p,
	                                          const utf16_t*   end );
	
}

#endif
