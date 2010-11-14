/*
	CFString_UTF8.cc
	----------------
*/

// chars
#include "encoding/utf8.hh"

// Carbonate
#include "Carbonate/CFString_UTF8.hh"


UInt32 count_UTF8_chars( const char* begin, const char* end )
{
	UInt32 result = 0;
	
	while ( begin < end )
	{
		const unsigned n_bytes = chars::count_utf8_bytes_in_char( *begin );
		
		if ( n_bytes == 0  ||  begin + n_bytes > end )
		{
			return 0;
		}
		
		begin += n_bytes;
		
		++result;
	}
	
	return result;
}


unsigned put_code_point_into_UTF8( unsigned int uc, char* p, const char* end )
{
	const unsigned size = chars::measure_utf8_bytes_for_unicode( uc );
	
	if ( p + size > end )
	{
		return 0;
	}
	
	chars::put_code_point_into_utf8( uc, size, p );
	
	return size;
}

