/*	==========
 *	Caption.cc
 *	==========
 */

#include "Pedestal/Caption.hh"

// Mac OS X
#ifdef __APPLE__
// We need this to define MAC_OS_X_VERSION_*
#include <CoreServices/CoreServices.h>
#endif

// missing-macos
#ifdef MAC_OS_X_VERSION_10_7
#ifndef MISSING_QUICKDRAWTEXT_H
#include "missing/QuickdrawText.h"
#endif
#endif

// nucleus
#include "nucleus/saved.hh"

// Nitrogen
#include "Nitrogen/Quickdraw.hh"
#include "Nitrogen/TextEdit.hh"


namespace Pedestal
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	
	
	static
	short get_TextMode()
	{
	#if OPAQUE_TOOLBOX_STRUCTS
		
		return GetPortTextMode( GetQDGlobalsThePort() );
		
	#else
		
		return ::qd.thePort->txMode;
		
	#endif
	}
	
	void Caption::Draw( const Rect& bounds, bool erasing )
	{
		n::saved< N::Clip > savedClip;
		
		Rect newBounds = bounds;
		
		if ( !Wrapped() )
		{
			newBounds.right = 30000;
			
			N::ClipRect( bounds );
		}
		
		const short saved_txMode = get_TextMode();
		
		if ( Disabled() )
		{
			::TextMode( grayishTextOr );
		}
		
		N::TETextBox( Text(), newBounds );
		
		::TextMode( saved_txMode );
	}
	
}
