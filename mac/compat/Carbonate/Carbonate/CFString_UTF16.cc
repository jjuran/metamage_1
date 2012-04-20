/*
	CFString_UTF16.cc
	-----------------
*/

// Carbonate
#include "Carbonate/CFString_UTF16.hh"


static inline bool is_leading_surrogate( UTF16Char uc )
{
	return uc >= 0xD800  &&  uc < 0xDC00;
}

static inline bool is_trailing_surrogate( UTF16Char uc )
{
	return uc >= 0xDC00  &&  uc < 0xE000;
}

UInt32 count_UTF16_chars( const UTF16Char* begin, const UTF16Char* end )
{
	UInt32 result = 0;
	
	while ( begin < end )
	{
		if ( is_trailing_surrogate( *begin ) )
		{
			return 0;
		}
		else if ( is_leading_surrogate( *begin ) )
		{
			if ( ++begin >= end  ||  !is_trailing_surrogate( *begin ) )
			{
				return 0;
			}
		}
		
		++begin;
		++result;
	}
	
	return result;
}

unsigned int get_next_code_point_from_UTF16( const char*& p, const char* end )
{
	if ( p + 2 > end )
	{
		return UTF32Char( -1 );
	}
	
	const UTF16Char*& u = (const UTF16Char *&) p;
	
	UTF32Char result = UTF32Char( -1 );
	
	if ( is_leading_surrogate( u[0] ) )
	{
		if ( is_trailing_surrogate( u[1] ) )
		{
			result = (u[0] & 0x3FF) << 10
			       | (u[1] & 0x3FF);
			
			result += 0x10000;
			
			u += 2;
		}
		else
		{
			// invalid unpaired surrogate
		}
	}
	else if ( is_trailing_surrogate( *u ) )
	{
		// invalid unpaired surrogate
	}
	else
	{
		result = *u++;
	}
	
	return result;
}

unsigned put_code_point_into_UTF16( unsigned int uc, char* p, const char* end )
{
	if ( p + 2 > end )
	{
		return 0;
	}
	
	if ( uc >= 0xD800  &&  uc < 0xE000 )
	{
		// invalid surrogate
		return 0;
	}
	
	UTF16Char*& u = (UTF16Char *&) p;
	
	if ( uc >= 0x10000 )
	{
		if ( uc >= 0x110000 )
		{
			return 0;
		}
		
		u[0] = 0xD800 | uc - 0x10000 >> 10;
		u[1] = 0xDC00 | uc & 0x3FF;
		
		return 2 * sizeof (UTF16Char);
	}
	
	*u = uc;
	
	return sizeof (UTF16Char);
}

