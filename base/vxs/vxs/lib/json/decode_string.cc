/*
	json/decode_string.cc
	---------------------
*/

#include "vxs/lib/json/decode_string.hh"

// iota
#include "iota/char_types.hh"

// chars
#include "encoding/utf8.hh"

// gear
#include "gear/hexadecimal.hh"

// vxo
#include "vxo/error.hh"

// vxs
#include "vxs/string.hh"


#define OutOfMemory Error( out_of_memory )


namespace vxo
{
namespace json
{

using chars::unichar_t;

static
unichar_t decode_unicode_bmp_escape( const char*& p )
{
	using iota::is_xdigit;
	using gear::decoded_hex_digit;
	
	uint8_t c1, c2, c3, c4;
	
	const bool xdigits = is_xdigit( c1 = *p++ )  &&
	                     is_xdigit( c2 = *p++ )  &&
	                     is_xdigit( c3 = *p++ )  &&
	                     is_xdigit( c4 = *p++ );
	
	if ( ! xdigits )
	{
		return (unichar_t) -1;
	}
	
	return + decoded_hex_digit( c1 ) << 12
	       | decoded_hex_digit( c2 ) <<  8
	       | decoded_hex_digit( c3 ) <<  4
	       | decoded_hex_digit( c4 ) <<  0;
}

enum
{
	control_character,
	invalid_escape,
	invalid_xdigit,
	high_surrogate,
	low_surrogate,
};

static const char* const string_literal_errors[] =
{
	"forbidden control character",
	"invalid escape metacharacter",
	"invalid hexadecimal digit",
	"unpaired high surrogate",
	"unpaired low surrogate",
};

static
int32_t decoded_string_length( const char* p )
{
	// The iterator points to the first byte after the opening quote.
	
	size_t length = 0;
	
	char c;
	
	while ( (c = *p++) != '"' )
	{
		if ( (uint8_t) c < 32 )
		{
			return ~control_character;
		}
		
		if ( c == '\\' )
		{
			switch ( *p++ )
			{
				case '/':
				case '"':
				case '\\':
				case 'b':
				case 't':
				case 'n':
				case 'f':
				case 'r':
					break;
				
				case 'u':
					unichar_t uc;
					
					uc = decode_unicode_bmp_escape( p );
					
					if ( (int32_t) uc < 0 )
					{
						return ~invalid_xdigit;
					}
					
					if ( (uc & 0xFC00) == 0xD800 )
					{
						// It's a high surrogate.  Get the low surrogate.
						
						unichar_t high = uc;
						
						uc = *p++ == '\\'  &&  *p++ == 'u'
						   ? decode_unicode_bmp_escape( p )
						   : 0;
						
						if ( (uc & 0xFC00) == 0xDC00 )
						{
							// We have a surrogate pair.
							
							uc &= 0x3FF;
							uc |= (high & 0x3FF) << 10;
							uc += 0x10000;
						}
						else
						{
							return ~high_surrogate;
						}
					}
					else if ( (uc & 0xFC00) == 0xDC00 )
					{
						return ~low_surrogate;
					}
					
					length += chars::measure_utf8_bytes_for_unicode( uc );
					
					continue;
					
				default:
					return ~invalid_escape;
			}
		}
		
		++length;
	}
	
	return length;
}

Box decode_string( const char*& p )
{
	// The iterator points to the first byte after the opening quote.
	
	int32_t length = decoded_string_length( p );
	
	if ( length < 0 )
	{
		return Error( string_literal_errors[ ~length ] );
	}
	
	plus::string s;
	
	char* q = s.reset_nothrow( length );
	
	if ( ! q )
	{
		return OutOfMemory;
	}
	
	char c;
	
	while ( (c = *p++) != '"' )
	{
		if ( c == '\\' )
		{
			c = *p++;
			
			switch ( c )
			{
				case '/':
				case '"':
				case '\\':
					break;
				
				case 'b':  c = '\b';  break;
				case 't':  c = '\t';  break;
				case 'n':  c = '\n';  break;
				case 'f':  c = '\f';  break;
				case 'r':  c = '\r';  break;
				
				case 'u':
					unichar_t uc;
					
					uc = decode_unicode_bmp_escape( p );
					
					if ( (uc & 0xFC00) == 0xD800 )
					{
						// It's a high surrogate.  Get the low surrogate.
						
						unichar_t high = uc;
						
						p += 2;
						
						uc = decode_unicode_bmp_escape( p );
						
						// We have a surrogate pair.
						
						uc &= 0x3FF;
						uc |= (high & 0x3FF) << 10;
						uc += 0x10000;
					}
					
					int n;
					n = chars::measure_utf8_bytes_for_unicode( uc );
					q = chars::put_code_point_into_utf8( uc, n, q );
					
					continue;
				
				default:
					// Not reached
					break;
			}
		}
		
		*q++ = c;
	}
	
	return String( s );
}

}
}
