/*
	quote.cc
	--------
*/

#include "vlib/quote.hh"

// iota
#include "iota/char_types.hh"
#include "iota/endian.hh"

// chars
#include "encoding/utf8.hh"

// gear
#include "gear/hexadecimal.hh"

// vxo
#include "vxo/lib/quote.hh"

// vlib
#include "vlib/throw.hh"


namespace vlib
{
	
	typedef plus::string::size_type size_t;
	
	static
	char decode_octal_escape( char c, const char*& p, char quote )
	{
		c -= '0';
		
		if ( c == 0  &&  *p == quote )
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
				THROW( "invalid octal escape sequence" );
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
			THROW( "invalid hexadecimal escape sequence" );
		}
		
		return + decoded_hex_digit( c1 ) << 4
		       | decoded_hex_digit( c2 ) << 0;
	}
	
	static
	void inscribe_unicode( char*& q, chars::unichar_t uc )
	{
		using chars::measure_utf8_bytes_for_unicode;
		using chars::put_code_point_into_utf8;
		
		const unsigned n = measure_utf8_bytes_for_unicode( uc );
		
		if ( n == 0 )
		{
			THROW( "invalid Unicode code point" );
		}
		
		put_code_point_into_utf8( uc, n, q );
		
		q += n;
	}
	
	static
	void decode_braced_unicode_escape( char*& q, const char*& p )
	{
		using iota::is_xdigit;
		using gear::decoded_hex_digit;
		
		while ( true )
		{
			bool has_digits = false;
			chars::unichar_t uc = 0;
			
			char c;
			
			while ( is_xdigit( c = *p++ ) )
			{
				if ( uc > 0x0FFFFFFF )
				{
					THROW( "invalid unicode escape sequence" );
				}
				
				uc <<= 4;
				uc |= decoded_hex_digit( c );
				
				has_digits = true;
			}
			
			if ( ! has_digits )
			{
				THROW( "invalid unicode escape sequence" );
			}
			
			inscribe_unicode( q, uc );
			
			if ( c == ' '  ||  c == '\t' )
			{
				continue;
			}
			
			if ( c != '}' )
			{
				THROW( "invalid unicode escape sequence" );
			}
			
			break;
		}
	}
	
	static
	chars::unichar_t decode_unicode_bmp_escape( const char*& p )
	{
		using iota::is_xdigit;
		using gear::decoded_hex_digit;
		
		const unsigned char c1 = *p++;
		const unsigned char c2 = *p++;
		
		if ( ! is_xdigit( c1 )  ||  ! is_xdigit( c2 ) )
		{
			THROW( "invalid unicode escape sequence" );
		}
		
		const unsigned char c3 = *p++;
		const unsigned char c4 = *p++;
		
		if ( ! is_xdigit( c3 )  ||  ! is_xdigit( c4 ) )
		{
			THROW( "invalid unicode escape sequence" );
		}
		
		return + decoded_hex_digit( c1 ) << 12
		       | decoded_hex_digit( c2 ) <<  8
		       | decoded_hex_digit( c3 ) <<  4
		       | decoded_hex_digit( c4 ) <<  0;
	}
	
	static
	void decode_unicode_escape( char*& q, const char*& p )
	{
		using chars::unichar_t;
		
		if ( *p == '{' )
		{
			decode_braced_unicode_escape( q, ++p );
			return;
		}
		
		const unichar_t uc = decode_unicode_bmp_escape( p );
		
		inscribe_unicode( q, uc );
	}
	
	char decode_escaped_byte( const char*& p )
	{
		using iota::is_alpha;
		using iota::is_digit;
		
		char c = *p++;
		
		if ( is_alpha( c ) )
		{
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
					THROW( "invalid escape sequence" );
			}
		}
		else if ( is_digit( c ) )
		{
			if ( c > '3' )
			{
				THROW( "invalid numeric escape sequence" );
			}
			
			c = decode_octal_escape( c, p, '\'' );
		}
		
		return c;
	}
	
	unsigned count_quoted_bytes( const char* p )
	{
		unsigned n = 0;
		
		while ( *p != '\'' )
		{
			++n;
			
			if ( *p++ == '\\' )
			{
				decode_escaped_byte( p );
			}
		}
		
		return n;
	}
	
	uint32_t unquote_mb32( const char* p )
	{
		uint32_t result = 0;
		
		char* q = (char*) &result;
		
		while ( *p != '\'' )
		{
			unsigned char c = *p++;
			
			if ( c == '\\' )
			{
				c = decode_escaped_byte( p );
			}
			
			*q++ = c;
		}
		
		return iota::u32_from_big( result );
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
						decode_unicode_escape( q, p );
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
							THROW( "invalid escape sequence" );
					}
				}
				else if ( is_digit( c ) )
				{
					if ( c > '3' )
					{
						THROW( "invalid numeric escape sequence" );
					}
					
					c = decode_octal_escape( c, p, '"' );
				}
			}
			
			*q++ = c;
		}
		
		// Cancel out the closing quotation mark.
		--q;
		
		return result.substr( 0, q - begin );
	}
	
	plus::string quote_string( const plus::string& s )
	{
		using vxo::quoted_length;
		
		const char* p = s.data();
		size_t      n = s.size();
		
		plus::string result;
		
		char* q = result.reset( quoted_length( p, n ) );
		
		vxo::quote_string( p, n, q );
		
		return result;
	}
	
}
