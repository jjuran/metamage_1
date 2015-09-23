/*
	quote.cc
	--------
*/

#include "vc/quote.hh"

// iota
#include "iota/char_types.hh"

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
	
}
