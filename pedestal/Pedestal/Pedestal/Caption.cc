/*	==========
 *	Caption.cc
 *	==========
 */

#include "Pedestal/Caption.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __TEXTEDIT__
#include <TextEdit.h>
#endif

// missing-macos
#ifdef MAC_OS_X_VERSION_10_7
#ifndef MISSING_QUICKDRAWTEXT_H
#include "missing/QuickdrawText.h"
#endif
#endif

// mac-sys-utils
#include "mac_sys/gestalt.hh"

// mac-qd-utils
#include "mac_qd/globals/patterns.hh"
#include "mac_qd/scoped_clipRect.hh"


#pragma exceptions off


namespace Pedestal
{
	
	static inline
	void TETextBox( const plus::string&  text,
	                const Rect&          box )
	{
		TETextBox( text.data(),
		           text.size(),
		           &box,
		           teFlushLeft );
	}
	
	static inline
	short get_TextMode()
	{
	#if OPAQUE_TOOLBOX_STRUCTS
		
		return GetPortTextMode( GetQDGlobalsThePort() );
		
	#else
		
		return ::qd.thePort->txMode;
		
	#endif
	}
	
	static inline
	long has_grayishTextOr()
	{
		enum
		{
			gestaltQuickdrawFeatures = 'qdrw',
			
			gestaltHasGrayishTextOr = 3,
		};
		
		long features = mac::sys::gestalt( gestaltQuickdrawFeatures );
		
		return features & (1 << gestaltHasGrayishTextOr);
	}
	
	void Caption::Draw( const Rect& bounds, bool erasing )
	{
		static RgnHandle tmp = NewRgn();
		
		static long has_grayishTextOr_mode = has_grayishTextOr();
		
		mac::qd::scoped_clipRect clipRect( bounds, tmp );
		
		Rect newBounds = bounds;
		
		if ( !Wrapped() )
		{
			newBounds.right = 30000;
		}
		
		const short saved_txMode = get_TextMode();
		
		bool disabled = Disabled();
		
		if ( disabled  &&  has_grayishTextOr_mode )
		{
			::TextMode( grayishTextOr );
		}
		
		TETextBox( Text(), newBounds );
		
		if ( disabled  &&  ! has_grayishTextOr_mode )
		{
			PenPat( &mac::qd::gray() );
			PenMode( patBic );
			
			PaintRect( &bounds );
			
			PenNormal();
		}
		
		::TextMode( saved_txMode );
	}
	
}
