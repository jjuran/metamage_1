// Nitrogen/Str.cp
// ---------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2002-2008 by Lisa Lippincott and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#include "Nitrogen/Str.h"

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
