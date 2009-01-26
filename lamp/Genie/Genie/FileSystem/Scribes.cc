/*	==========
 *	Scribes.cc
 *	==========
 */

#include "Genie/FileSystem/Scribes.hh"

// Nucleus
#include "Nucleus/Convert.h"


namespace Genie
{
	
	namespace NN = Nucleus;
	
	
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
		std::string result = NN::Convert< std::string >( point.h );
		
		result += separator;
		
		result += NN::Convert< std::string >( point.v );
		
		return result;
	}
	
}

