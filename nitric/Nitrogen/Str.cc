// Nitrogen/Str.cc
// ---------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2002-2008 by Lisa Lippincott and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#include "Nitrogen/Str.hh"

// Standard C++
#include <algorithm>


namespace Nitrogen
{
	
	void CopyToPascalString( const char     *source,
	                         std::size_t     sourceLength,
	                         unsigned char  *destination,
	                         unsigned char   destinationLength )
	{
		if ( sourceLength > destinationLength )
		{
			throw StringTooLong();
		}
		
		destination[ 0 ] = static_cast< unsigned char >( sourceLength );
		
		std::copy( source,
		           source + sourceLength,
		           reinterpret_cast< char * >( destination + 1 ) );
     }
     
}

namespace nucleus
{
	
	std::string
	//
	converter< std::string, const unsigned char * >::operator()( const unsigned char *input ) const
	{
		const char *begin = reinterpret_cast< const char * >( input + 1 );
		
		return std::string( begin, 0 + input[ 0 ] ); 
	}
	
}

