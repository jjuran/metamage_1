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


#ifndef NITROGEN_TEXTEDIT_H
#include "Nitrogen/TextEdit.h"
#endif

// Standard C++
#include <algorithm>

// Nitrogen
#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif


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

