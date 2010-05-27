// Nitrogen/TextEdit.cc
// --------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#include "Nitrogen/TextEdit.hh"

// Nitrogen
#include "Nitrogen/OSStatus.hh"


namespace Nitrogen
{
	
	nucleus::owned< TEHandle > TENew( const Rect&  destRect,
	                                  const Rect&  viewRect )
	{
		TEHandle hTE = ::TENew( &destRect, &viewRect );
		
		// Just a guess
		MemError();
		
		if ( hTE == NULL )
		{
			ThrowOSStatus( memFullErr );
		}
		
		return nucleus::owned< TEHandle >::seize( hTE );
	}
	
}

