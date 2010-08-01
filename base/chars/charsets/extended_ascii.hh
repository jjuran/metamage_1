/*
	extended_ascii.hh
	-----------------
*/

#ifndef CHARSETS_EXTENDEDASCII_HH
#define CHARSETS_EXTENDEDASCII_HH

// charsets
#include "charsets/unicode.hh"


namespace chars
{
	
	// attempt to convert a Unicode character not present in this character set
	class unrepresentable_character {};
	
	struct unicode_mapping
	{
		unsigned short  unicode;
		unsigned char   code;
	};
	
	enum
	{
		n_extended_ASCII_mappings = 128
	};
	
	
	inline unichar_t unicode_from_extended_ascii( unsigned char c, const unsigned short* table )
	{
		return c < 0x80 ? c
		                : table[ c & 0x7F ];
	}
	
	char extended_ascii_from_unicode( unichar_t              uc,
	                                  const unicode_mapping  map[] );
	
}

#endif

