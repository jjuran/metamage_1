/*
	vivify_qd.cc
	------------
*/

#include "Genie/FS/vivify_qd.hh"

// Standard C
#include <ctype.h>

// gear
#include "gear/parse_decimal.hh"


namespace Genie
{
	
	using mac::types::Point;
	using mac::types::Rect;
	
	
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
		
		result.h = gear::parse_decimal( &begin );
		
		advance_to_decimal( begin, end );
		
		result.v = gear::parse_decimal( &begin );
		
		return result;
	}
	
	Rect vivify_Rect::apply( const char* begin, const char* end )
	{
		Rect result;
		
		result.left = gear::parse_decimal( &begin );
		
		advance_to_decimal( begin, end );
		
		result.top = gear::parse_decimal( &begin );
		
		advance_to_decimal( begin, end );
		
		result.right = gear::parse_decimal( &begin );
		
		advance_to_decimal( begin, end );
		
		result.bottom = gear::parse_decimal( &begin );
		
		return result;
	}
	
}

