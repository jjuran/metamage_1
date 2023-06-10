/*
	View.cc
	-------
*/

#include "Pedestal/View.hh"

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// missing-macos
#ifdef MAC_OS_X_VERSION_10_7
#ifndef MISSING_QUICKDRAW_H
#include "missing/Quickdraw.h"
#endif
#endif

// Pedestal
#include "Pedestal/ClickTarget.hh"
#include "Pedestal/CurrentFocus.hh"


namespace Pedestal
{
	
	void View::Uninstall()
	{
		// This is only needed for focusable fields, but it's critical to ensure
		// that we don't continue to reference them after they've been destroyed.
		// Rather than count on remembering to override Uninstall() for every
		// field class, just do it here.
		
		Unfocus( this );
		Untarget( this );
	}
	
	void View::Draw( const Rect& bounds, bool erasing )
	{
		if ( erasing )
		{
			EraseRect( &bounds );
		}
	}
	
	void View::DrawInContext( CGContextRef context, CGRect bounds )
	{
	}
	
	TextEdit* View::EnterShiftSpaceQuasimode( const EventRecord& )
	{
		return NULL;
	}
	
}
