/*
	encode_string.cc
	----------------
*/

#include "vxo-string/lib/json/encode_string.hh"

// iota
#include "iota/char_types.hh"

// gear
#include "gear/hexadecimal.hh"

// vxo
#include "vxo/error.hh"

// vxo-string
#include "vxo-string/string.hh"


#pragma exceptions off


/*
	There's no way to make this not be a macro
	without Metrowerks generating larger code.
*/

#define OutOfMemory Error( out_of_memory )


namespace vxo  {
namespace json {

static inline
bool backslash_escapable( uint8_t c )
{
	return (c >= '\b' && c <= '\r'  &&  c != '\v')  ||  c == '\\'  ||  c == '"';
}

static
plus::string::size_type quoted_length( const char* p, unsigned n )
{
	typedef plus::string::size_type size_t;
	
	plus::string::size_type length = 2;  // the enclosing quotes
	
	const char* end = p + n;
	
	while ( p < end )
	{
		unsigned char c = *p++;
		
		if ( backslash_escapable( c ) )
		{
			length += 2;  // e.g. "\n" or "\\"
		}
		else if ( iota::is_cntrl( c ) )
		{
			length += 6;  // e.g. "\u0000"
		}
		else if ( iota::is_ascii( c ) )
		{
			length += 1;
		}
		else if ( c < 0xF0 )
		{
			p += 1 + (c >= 0xE0);
			
			length += 6;  // e.g. "\u2022"
		}
		else
		{
			p += 3;
			
			length += 12;  // e.g. "\uD83D\uDCA9"
		}
	}
	
	return length;
}

Box encode_string( const char* p, unsigned n )
{
	plus::string result;
	
	char* q = result.reset_nothrow( quoted_length( p, n ) );
	
	if ( ! q )
	{
		return OutOfMemory;
	}
	
	*q++ = '"';
	
	while ( n-- > 0 )
	{
		unsigned char c = *p++;
		
		if ( iota::is_ascii( c )  &&  ! iota::is_cntrl( c ) )
		{
			if ( c == '\\'  ||  c == '"' )
			{
				*q++ = '\\';
			}
			
			*q++ = c;
			continue;
		}
		
		*q++ = '\\';
		
		if ( c >= '\b'  &&  c <= '\r'  &&  c != '\v' )
		{
			const char* escapes = "btn fr";
			
			*q++ = escapes[ c - '\b' ];
			continue;
		}
		
		*q++ = 'u';
		
		if ( iota::is_ascii( c ) )
		{
			*q++ = '0';
			*q++ = '0';
			*q++ = gear::encoded_hex_char( c >> 4 );
			*q++ = gear::encoded_hex_char( c >> 0 );
		}
		else if ( n-- > 0 )
		{
			uint8_t c1 = *p++;
			
			if ( c < 0xE0 )
			{
				uint16_t uc = (c  & 0x1f) << 6
				            | (c1 & 0x3f);
				
				*q++ = '0';
				*q++ = gear::encoded_hex_char( uc >> 8 );
				*q++ = gear::encoded_hex_char( uc >> 4 );
				*q++ = gear::encoded_hex_char( uc >> 0 );
			}
			else if ( n-- > 0 )
			{
				uint8_t c2 = *p++;
				
				if ( c < 0xF0 )
				{
					uint16_t uc = (c1 & 0x3f) <<  6
					            | (c2 & 0x3f);
					
					*q++ = gear::encoded_hex_char( c );
					*q++ = gear::encoded_hex_char( uc >> 8 );
					*q++ = gear::encoded_hex_char( uc >> 4 );
					*q++ = gear::encoded_hex_char( uc >> 0 );
				}
				else if ( n-- > 0 )
				{
					uint8_t c3 = *p++;
					
					if ( c < 0xF5 )
					{
						uint32_t uc = (c  & 0x07) << 18
						            | (c1 & 0x3f) << 12
						            | (c2 & 0x3f) <<  6
						            | (c3 & 0x3f);
						
						uc -= 0x10000;
						
						uint16_t s0 = 0xD800 | (uc >> 10 & 0x03FF);
						uint16_t s1 = 0xDC00 | (uc       & 0x03FF);
						
						*q++ = 'd';
						*q++ = gear::encoded_hex_char( s0 >> 8 );
						*q++ = gear::encoded_hex_char( s0 >> 4 );
						*q++ = gear::encoded_hex_char( s0 >> 0 );
						
						*q++ = '\\';
						*q++ = 'u';
						*q++ = 'd';
						*q++ = gear::encoded_hex_char( s1 >> 8 );
						*q++ = gear::encoded_hex_char( s1 >> 4 );
						*q++ = gear::encoded_hex_char( s1 >> 0 );
					}
				}
			}
		}
	}
	
	*q++ = '"';
	
	return String( result );
}

}
}
