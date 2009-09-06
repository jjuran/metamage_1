/*	==========
 *	Scribes.cc
 *	==========
 */

#include "Genie/FS/Scribes.hh"

// Iota
#include "iota/decimal.hh"


namespace Genie
{
	
	std::string Int_Scribe::Encode( int value )
	{
		return iota::inscribe_decimal( value );
	}
	
	static inline bool is_integer( const char* s )
	{
		return std::isdigit( s[ s[0] == '-' ] );
	}
	
	Point ReadPoint( const char* begin, const char* end )
	{
		const char* p = begin;
		
		const long x = std::strtol( p, (char**) &p, 10 );
		
		if ( p != begin  &&  p != end )
		{
			while ( *++p )
			{
				if ( is_integer( p ) )
				{
					const long y = std::strtol( p, NULL, 10 );
					
					const Point result = { y, x };
					
					return result;
				}
			}
		}
		
		throw Invalid_Decode_Input();
	}
	
	std::string WritePoint( Point point, char separator )
	{
		std::string result = iota::inscribe_decimal( point.h );
		
		result += separator;
		
		result += iota::inscribe_decimal( point.v );
		
		return result;
	}
	
	std::string Rect_Scribe::Encode( const Rect& r )
	{
		std::string result;
		
		result += iota::inscribe_decimal( r.left );
		
		result += ",";
		
		result += iota::inscribe_decimal( r.top );
		
		result += "-";
		
		result += iota::inscribe_decimal( r.right );
		
		result += ",";
		
		result += iota::inscribe_decimal( r.bottom );
		
		return result;
	}
	
}

