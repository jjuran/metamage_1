/*
	escape.cc
	---------
*/

#include "escape.hh"

// iota
#include "iota/char_types.hh"


static
unsigned escaped_size( const char* in, unsigned len )
{
	if ( len == 0 )
	{
		/*
			The quoted string is ''.
		*/
		
		return 2;
	}
	
	const char* p = in;
	const char* q = in + len;
	
	unsigned n_quotes = 0;
	
	while ( p < q )
	{
		n_quotes += *p++ == '\'';
	}
	
	if ( n_quotes )
	{
		/*
			The quoted string consists of the original string,
			delimited by single quotes, with each single quote
			within the string replaced by a four-byte sequence.
		*/
		
		return len + 2 + n_quotes * 3;
	}
	
	p = in;
	
	while ( p < q )
	{
		if ( ! iota::is_alnum( *p++ ) )
		{
			/*
				The quoted string is just the original string plus quotes.
			*/
			
			return len + 2;
		}
	}
	
	/*
		The string doesn't need to be quoted at all.
	*/
	
	return 0;
}

static
void escape( char* out, unsigned out_len, const char* in, unsigned in_len )
{
	char* q = out;
	
	*q++ = '\'';
	
	const char* p = in;
	const char* end = in + in_len;
	
	unsigned n_quotes_x3 = out_len - in_len - 2;
	
	while ( n_quotes_x3 > 0 )
	{
		while ( *p != '\'' )
		{
			*q++ = *p++;
		}
		
		*q++ = *p++;
		*q++ = '\xB6';  // little delta
		*q++ = '\'';
		*q++ = '\'';
		
		n_quotes_x3 -= 3;
	}
	
	while ( p < end )
	{
		*q++ = *p++;
	}
	
	*q++ = '\'';
}

plus::string escaped( const plus::string& str )
{
	const char* data = str.data();
	unsigned    size = str.size();
	
	unsigned new_size = escaped_size( data, size );
	
	if ( new_size )
	{
		plus::string result;
		
		char* p = result.reset( new_size );
		
		escape( p, new_size, data, size );
		
		return result;
	}
	
	return str;
}
