// Nitrogen/TextEdit.cp
// --------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#include "Nitrogen/TextEdit.h"

// Standard C++
#include <algorithm>

// Nitrogen
#include "Nitrogen/OSStatus.h"


namespace Nitrogen
{
	
	Nucleus::Owned< TEHandle > TENew( const Rect&  destRect,
	                                  const Rect&  viewRect )
	{
		TEHandle hTE = ::TENew( &destRect, &viewRect );
		
		// Just a guess
		MemError();
		
		if ( hTE == NULL )
		{
			ThrowOSStatus( memFullErr );
		}
		
		return Nucleus::Owned< TEHandle >::Seize( hTE );
	}
	
}

