/*
	quote.cc
	--------
*/

#include "vxo/lib/quote.hh"

// iota
#include "iota/char_types.hh"

// gear
#include "gear/hexadecimal.hh"


namespace vxo
{

unsigned quoted_length( unsigned char c )
{
	const unsigned n = 2;  // the enclosing quotes
	
	if ( (c >= '\a'  &&  c <= '\r')  ||  c == '\\'  ||  c == '\'' )
	{
		return n + 2;  // e.g. '\n' or '\\'
	}
	
	if ( iota::is_cntrl( c )  ||  ! iota::is_ascii( c ) )
	{
		return n + 4;  // e.g. '\x00' or '\xFF'
	}
	
	return n + 1;
}

unsigned quoted_length( const char* p, unsigned n )
{
	unsigned length = 2;  // the enclosing quotes
	
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

char* copy_quotable_byte( char* q, unsigned char c )
{
	if ( ! iota::is_cntrl( c )  &&  iota::is_ascii( c ) )
	{
		if ( c == '\\'  ||  c == '\'' )
		{
			*q++ = '\\';
		}
		
		*q++ = c;
	}
	else
	{
		*q++ = '\\';
		
		if ( c >= '\a'  &&  c <= '\r' )
		{
			const char* escapes = "abtnvfr";
			
			*q++ = escapes[ c - '\a' ];
		}
		else
		{
			*q++ = 'x';
			*q++ = gear::encoded_hex_char( c >> 4 );
			*q++ = gear::encoded_hex_char( c >> 0 );
		}
	}
	
	return q;
}

char* quote_byte( char* q, unsigned char c )
{
	*q++ = '\'';
	
	q = copy_quotable_byte( q, c );
	
	*q++ = '\'';
	
	return q;
}

char* quote_string( const char* p, unsigned n, char* q )
{
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
	
	return q;
}

}
