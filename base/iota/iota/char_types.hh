/*
	char_types.hh
	-------------
*/

#ifndef IOTA_CHARTYPES_HH
#define IOTA_CHARTYPES_HH


namespace iota
{
	
	inline bool is_ascii( char c )
	{
		const signed char sc = c;
		
		return sc >= 0;
	}
	
	inline bool is_cntrl( char c )
	{
		const unsigned char uc = c;
		
		return uc <= 0x1f  ||  uc == 0x7f;
	}
	
	inline bool is_blank( char c )
	{
		return c == ' '  ||  c == '\t';
	}
	
	inline bool is_space( char c )
	{
		const unsigned char uc = c;
		
		/*
			0x09  \t
			0x0a  \n
			0x0b  \v
			0x0c  \f
			0x0d  \r
		*/
		
		return uc == ' '  ||  (uc <= 0x0d  &&  uc >= 0x09);
	}
	
	inline bool is_print( char c )
	{
		const signed char sc = c;
		
		return sc >= ' '  &&  sc < 0x7f;
	}
	
	inline bool is_graph( char c )
	{
		const signed char sc = c;
		
		return sc > ' '  &&  sc < 0x7f;
	}
	
	inline bool is_upper( char c )
	{
		const unsigned char uc = c;
		
		return uc <= 'Z'  &&  uc >= 'A';
	}
	
	inline bool is_lower( char c )
	{
		const signed char sc = c;
		
		return sc >= 'a'  &&  sc <= 'z';
	}
	
	inline bool is_alpha( char c )
	{
		const signed char sc = c | 0x20;
		
		return sc >= 'a'  &&  sc <= 'z';
	}
	
	inline bool is_digit( char c )
	{
		const unsigned char uc = c;
		
		return uc <= '9'  &&  uc >= '0';
	}
	
	inline bool is_xdigit( char c )
	{
		const signed char sc = c;
		
		if ( sc < '0' )
		{
			return false;
		}
		
		if ( sc <= '9' )
		{
			return true;
		}
		
		c |= 0x20;
		
		return c >= 'a'  &&  c <= 'f';
	}
	
	inline bool is_alnum( char c )
	{
		const signed char sc = c;
		
		if ( sc < '0' )
		{
			return false;
		}
		
		if ( sc <= '9' )
		{
			return true;
		}
		
		c |= 0x20;
		
		return c >= 'a'  &&  c <= 'z';
	}
	
	inline bool is_punct( char c )
	{
		return !is_alnum( c )  &&  is_graph( c );
	}
	
	inline char _to_upper( char c )
	{
		return c - 0x20;
	}
	
	inline char _to_lower( char c )
	{
		return c + 0x20;
	}
	
	inline char to_upper( char c )
	{
		if ( is_lower( c ) )
		{
			c -= 0x20;
		}
		
		return c;
	}
	
	inline char to_lower( char c )
	{
		if ( is_upper( c ) )
		{
			c += 0x20;
		}
		
		return c;
	}
	
}

#endif
