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

// Standard C/C++
#include <cstring>


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
		
		std::memcpy( destination + 1, source, sourceLength );
     }
     
}

namespace nucleus
{
	
	nucleus::string
	//
	converter< nucleus::string, const unsigned char * >::operator()( const unsigned char *input ) const
	{
		const char *begin = reinterpret_cast< const char * >( input + 1 );
		
		return nucleus::string( begin, 0 + input[ 0 ] ); 
	}
	
}

