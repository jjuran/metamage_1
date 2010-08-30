/*
	vivify_qd.cc
	------------
*/

#include "Genie/FS/vivify_qd.hh"

// Standard C
#include <ctype.h>

// iota
#include "iota/decimal.hh"


namespace Genie
{
	
	static void advance_to_decimal( const char*& p, const char* end )
	{
		if ( p < end )
		{
			while ( ++p < end )
			{
				if ( isdigit( p[ p[ 0 ] == '-' ] ) )
				{
					return;
				}
			}
		}
		
		throw invalid_decode_input_error();
	}
	
	Point vivify_Point::apply( const char* begin, const char* end )
	{
		Point result;
		
		result.h = iota::parse_decimal( &begin );
		
		advance_to_decimal( begin, end );
		
		result.v = iota::parse_decimal( &begin );
		
		return result;
	}
	
	Rect vivify_Rect::apply( const char* begin, const char* end )
	{
		Rect result;
		
		result.left = iota::parse_decimal( &begin );
		
		advance_to_decimal( begin, end );
		
		result.top = iota::parse_decimal( &begin );
		
		advance_to_decimal( begin, end );
		
		result.right = iota::parse_decimal( &begin );
		
		advance_to_decimal( begin, end );
		
		result.bottom = iota::parse_decimal( &begin );
		
		return result;
	}
	
}

