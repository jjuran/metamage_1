/*	=================
 *	CGDirectDisplay.h
 *	=================
 */

#ifndef NITROGEN_CGDIRECTDISPLAY_H
#define NITROGEN_CGDIRECTDISPLAY_H

// Mac OS
#include <Carbon/Carbon.h>
#include <ApplicationServices/ApplicationServices.h>
	
// Nitrogen
#ifndef NITROGEN_OWNED_H
#include "Nitrogen/Owned.h"
#endif


namespace Nitrogen
{
	
	using ::CGDirectDisplayID;
	
	template <> struct Disposer< CGDirectDisplayID > : public std::unary_function< CGDirectDisplayID, void >
	{
		void operator()( CGDirectDisplayID display ) const
		{
			::CGDisplayRelease( display );
		}
	};
	
	// This belongs in Nitrogen/Quickdraw.h, but it depends on CGDirectDisplayID,
	// which is defined in CGDirectDisplay.h which isn't included by Quickdraw.h,
	// so we move it here so as not to create a new header dependency.
	// (It's defined in Nitrogen/Quickdraw.cp.)
	Owned< CGrafPtr > CreateNewPortForCGDisplayID( CGDirectDisplayID display );
	
	Owned< CGDirectDisplayID > CGDisplayCapture( CGDirectDisplayID display = 0 );
	
	inline void CGDisplayRelease( Owned< CGDirectDisplayID > )  {}
	
}

#endif

