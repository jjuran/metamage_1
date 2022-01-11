/*
	utf8.hh
	-------
*/

#ifndef ENCODING_UTF8_HH
#define ENCODING_UTF8_HH

// charsets
#include "charsets/unicode.hh"


namespace chars
{
	
	typedef unsigned char utf8_t;
	
	inline unsigned measure_utf8_bytes_for_unicode( unichar_t uc )
	{
		return   uc <     0x80 ? 1  // ASCII
		       : uc <    0x800 ? 2
		       : uc <   0xD800 ? 3
		       : uc <   0xE000 ? 0  // D800 - DFFF are for surrogate pairs only
		       : uc <  0x10000 ? 3
		       : uc < 0x110000 ? 4
		       :                 0;
	}
	
	inline unsigned count_utf8_bytes_in_char( utf8_t c )
	{
		return   c < 0x80 ? 1
		       : c < 0xC0 ? 0  // continuation byte
		       : c < 0xC2 ? 0  // overlong 2-byte encoding of code point < 128
		       : c < 0xE0 ? 2
		       : c < 0xF0 ? 3
		       : c < 0xF5 ? 4
		       :            0;  // reserved or invalid
	}
	
	char* put_code_point_into_utf8( unichar_t uc, unsigned n_bytes, char* p );
	
	unichar_t get_next_code_point_from_utf8( const char*& p, const char* end );
	
}

#endif
