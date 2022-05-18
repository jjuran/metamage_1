/*
	plot_icon_id.cc
	---------------
*/

#include "mac_qd/plot_icon_id.hh"

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __ICONS__
#include <Icons.h>
#endif
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// mac-sys-utils
#include "mac_sys/has/IconUtilities.hh"
#include "mac_sys/res_error.hh"


namespace mac {
namespace qd  {

#if ! __LP64__

short plot_icon_id( const Rect& bounds, short id )
{
	if ( mac::sys::has_IconUtilities() )
	{
		return PlotIconID( &bounds, 0, 0, id );
	}
	else if ( Handle h = GetResource( 'ICN#', id ) )
	{
		PlotIcon( &bounds, h );
		
		return noErr;
	}
	else if ( OSErr err = mac::sys::res_error() )
	{
		return err;
	}
	
	return resNotFound;
}

#else

int dummy;

#endif

}
}
