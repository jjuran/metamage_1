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
	
	template < const unsigned short table[] >
	inline unichar_t get_next_code_point_from_extended_ascii( const char*& p, const char* end )
	{
		return unicode_from_extended_ascii( *p++, table );
	}
	
	
	char extended_ascii_from_unicode( unichar_t              uc,
	                                  const unicode_mapping  map[] );
	
	unsigned put_code_point_into_extended_ascii( unichar_t              uc,
	                                             char*                  p,
	                                             const unicode_mapping  map[] );
	
	template < const unicode_mapping map[] >
	inline unsigned put_code_point_into_extended_ascii( unichar_t uc, char* p, const char* end )
	{
		return put_code_point_into_extended_ascii( uc, p, map );
	}
	
}

#endif

