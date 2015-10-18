/*
	quote.cc
	--------
*/

#include "vc/quote.hh"

// iota
#include "iota/char_types.hh"

// chars
#include "encoding/utf8.hh"

// gear
#include "gear/hexadecimal.hh"

// vc
#include "vc/error.hh"


namespace vc
{
	
	static
	char decode_octal_escape( char c, const char*& p )
	{
		c -= '0';
		
		if ( c == 0  &&  *p == '"' )
		{
			// `\0` is allowed at the end of a string.
			return c;
		}
		else
		{
			const unsigned char c1 = *p++ - '0';
			const unsigned char c2 = *p++ - '0';
			
			if ( c1 > 7  ||  c2 > 7 )
			{
				SYNTAX_ERROR( "invalid octal escape sequence" );
			}
			
			return c * 64 + c1 * 8 + c2;
		}
	}
	
	static
	char decode_hex_escape( const char*& p )
	{
		using iota::is_xdigit;
		using gear::decoded_hex_digit;
		
		const unsigned char c1 = *p++;
		const unsigned char c2 = *p++;
		
		if ( ! is_xdigit( c1 )  ||  ! is_xdigit( c2 ) )
		{
			SYNTAX_ERROR( "invalid hexadecimal escape sequence" );
		}
		
		return + decoded_hex_digit( c1 ) << 4
		       | decoded_hex_digit( c2 ) << 0;
	}
	
	static
	chars::unichar_t decode_braced_unicode_escape( const char*& p )
	{
		using iota::is_xdigit;
		using gear::decoded_hex_digit;
		
		chars::unichar_t uc = 0;
		
		char c;
		
		while ( is_xdigit( c = *p++ ) )
		{
			if ( uc > 0x0FFFFFFF )
			{
				SYNTAX_ERROR( "invalid unicode escape sequence" );
			}
			
			uc <<= 4;
			uc |= decoded_hex_digit( c );
		}
		
		if ( c != '}' )
		{
			SYNTAX_ERROR( "invalid unicode escape sequence" );
		}
		
		return uc;
	}
	
	static
	chars::unichar_t decode_unicode_escape( const char*& p )
	{
		using iota::is_xdigit;
		using gear::decoded_hex_digit;
		
		if ( *p == '{' )
		{
			return decode_braced_unicode_escape( ++p );
		}
		
		const unsigned char c1 = *p++;
		const unsigned char c2 = *p++;
		
		if ( ! is_xdigit( c1 )  ||  ! is_xdigit( c2 ) )
		{
			SYNTAX_ERROR( "invalid unicode escape sequence" );
		}
		
		const unsigned char c3 = *p++;
		const unsigned char c4 = *p++;
		
		if ( ! is_xdigit( c3 )  ||  ! is_xdigit( c4 ) )
		{
			SYNTAX_ERROR( "invalid unicode escape sequence" );
		}
		
		return + decoded_hex_digit( c1 ) << 12
		       | decoded_hex_digit( c2 ) <<  8
		       | decoded_hex_digit( c3 ) <<  4
		       | decoded_hex_digit( c4 ) <<  0;
	}
	
	plus::string unquote_escaped_string( const plus::string& s )
	{
		plus::string result;
		
		/*
			Allocate enough space for the original string, minus the quotes.
			We actually do append the closing quote temporarily, but the
			result string is always shorter than the input, since there's at
			least one escape sequence, and every escape sequence is longer
			than the one byte to which it decodes.
		*/
		
		char* begin = result.reset( s.size() - 2 );
		char* q = begin;
		
		const char* p = s.c_str() + 1;
		
		while ( char c = *p++ )
		{
			if ( c == '\\' )
			{
				using iota::is_alpha;
				using iota::is_digit;
				
				c = *p++;
				
				if ( is_alpha( c ) )
				{
					if ( c == 'u' )
					{
						using chars::unichar_t;
						using chars::measure_utf8_bytes_for_unicode;
						using chars::put_code_point_into_utf8;
						
						const unichar_t uc = decode_unicode_escape( p );
						
						const unsigned n = measure_utf8_bytes_for_unicode( uc );
						
						if ( n == 0 )
						{
							SYNTAX_ERROR( "invalid Unicode code point" );
						}
						
						put_code_point_into_utf8( uc, n, q );
						
						q += n;
						
						continue;
					}
					
					switch ( c )
					{
						case 'a':  c = '\a';  break;
						case 'b':  c = '\b';  break;
						case 't':  c = '\t';  break;
						case 'n':  c = '\n';  break;
						case 'v':  c = '\v';  break;
						case 'f':  c = '\f';  break;
						case 'r':  c = '\r';  break;
						
						case 'x':
							c = decode_hex_escape( p );
							break;
						
						default:
							SYNTAX_ERROR( "invalid escape sequence" );
					}
				}
				else if ( is_digit( c ) )
				{
					if ( c > '3' )
					{
						SYNTAX_ERROR( "invalid numeric escape sequence" );
					}
					
					c = decode_octal_escape( c, p );
				}
			}
			
			*q++ = c;
		}
		
		// Cancel out the closing quotation mark.
		--q;
		
		return result.substr( 0, q - begin );
	}
	
	static
	plus::string::size_type quoted_length( const plus::string& s )
	{
		typedef plus::string::size_type size_t;
		
		const char* p = s.data();
		size_t      n = s.size();
		
		plus::string::size_type length = 2;  // the enclosing quotes
		
		while ( n-- > 0 )
		{
			unsigned char c = *p++;
			
			if ( (c >= '\a'  &&  c <= '\r')  ||  c == '\\'  ||  c == '"' )
			{
				length += 2;  // e.g. "\n" or "\\"
			}
			else if ( iota::is_cntrl( c ) )
			{
				length += 4;  // e.g. "\x00"
			}
			else
			{
				length += 1;
			}
		}
		
		return length;
	}
	
	plus::string quote_string( const plus::string& s )
	{
		typedef plus::string::size_type size_t;
		
		const char* p = s.data();
		size_t      n = s.size();
		
		plus::string result;
		
		char* q = result.reset( quoted_length( s ) );
		
		*q++ = '"';
		
		while ( n-- > 0 )
		{
			unsigned char c = *p++;
			
			if ( ! iota::is_cntrl( c ) )
			{
				if ( c == '\\'  ||  c == '"' )
				{
					*q++ = '\\';
				}
				
				*q++ = c;
				continue;
			}
			
			*q++ = '\\';
			
			if ( c >= '\a'  &&  c <= '\r' )
			{
				const char* escapes = "abtnvfr";
				
				*q++ = escapes[ c - '\a' ];
				continue;
			}
			
			*q++ = 'x';
			*q++ = gear::encoded_hex_char( c >> 4 );
			*q++ = gear::encoded_hex_char( c >> 0 );
		}
		
		*q++ = '"';
		
		return result;
	}
	
}
