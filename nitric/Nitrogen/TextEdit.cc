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


namespace Nitrogen
{
	
#if ! __LP64__
	
	nucleus::owned< TEHandle > TENew( const Rect&  destRect,
	                                  const Rect&  viewRect )
	{
		TEHandle hTE = ::TENew( &destRect, &viewRect );
		
		if ( hTE == NULL )
		{
			Mac::ThrowOSStatus( memFullErr );
		}
		
		return nucleus::owned< TEHandle >::seize( hTE );
	}
	
#else
	
	void dummy()
	{
	}
	
#endif  // #if ! __LP64__
	
}
