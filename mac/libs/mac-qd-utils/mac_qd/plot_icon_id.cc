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

// mac-qd-utils
#include "mac_qd/copy_bits.hh"
#include "mac_qd/globals/thePort.hh"


#pragma exceptions off


namespace mac {
namespace qd  {

enum
{
	Inverse = 2,
};

const Fixed fixed_72dpi = 72 << 16;

#if ! __LP64__

#if ! OPAQUE_TOOLBOX_STRUCTS

static inline
Boolean IsPortColor( GrafPtr port )
{
	return port->portBits.rowBytes < 0;
}

#endif

static inline
bool silhouetted( GrafPtr port )
{
	/*
		The port is assumed to be a basic (non-color) graphics port.
		
		This predicate is not correct for opaque Toolbox structs,
		nor (by extension) for Carbon.  However, it's only used as
		a fallback for Icon Utilities, which well precedes Carbon.
	*/
	
#if ! OPAQUE_TOOLBOX_STRUCTS
	
	return ! ((port->fgColor ^ port->bkColor) & Inverse);
	
#endif
	
	return false;
}

short plot_icon_id( const Rect& bounds, short id )
{
	if ( ! TARGET_CPU_68K  ||  mac::sys::has_IconUtilities() )
	{
		return PlotIconID( &bounds, 0, 0, id );
	}
	else if ( Handle h = GetResource( 'ICN#', id ) )
	{
		GrafPtr port = thePort();
		
		UInt32 icon_data[ 32 ];
		
		BlockMoveData( *h + 128, icon_data, sizeof icon_data );  // icon mask
		
		BitMap icon =
		{
			(Ptr) icon_data,
			4,
			{ 0, 0, 32, 32 },
		};
		
		copy_bits( icon, port, icon.bounds, bounds, srcBic );  // apply mask
		
		bool is_color = IsPortColor( port );
		
		Handle icl8;
		
		if ( is_color  &&  (icl8 = GetResource( 'icl8', id )) )
		{
			const short bpp     = 8;
			const short clut_id = bpp;
			
			if ( CTabHandle ctab = GetCTable( clut_id ) )
			{
				Byte hstate = HGetState( icl8 );
				
				HLock( icl8 );
				
				PixMap pixmap = {};
				
				pixmap.baseAddr = *icl8;
				pixmap.rowBytes = (short) 0x8000 | 32;  // 32px * 8bpp
				//pixmap.bounds.top  = 0;
				//pixmap.bounds.left = 0;
				pixmap.bounds.bottom = 32;
				pixmap.bounds.right  = 32;
				
				pixmap.vRes = fixed_72dpi;
				pixmap.hRes = fixed_72dpi;
				
				pixmap.pixelSize = bpp;
				pixmap.cmpCount  = 1;
				pixmap.cmpSize   = bpp;
				
				pixmap.pmTable = ctab;
				
				copy_bits( pixmap, port, pixmap.bounds, bounds, srcOr );
				
				HSetState( icl8, hstate );
				
				DisposeCTable( ctab );
				
				goto drawn;
			}
		}
		
		if ( ! is_color  &&  silhouetted( port ) )
		{
			goto drawn;
		}
		
		BlockMoveData( *h, icon_data, sizeof icon_data );  // icon face
		
		copy_bits( icon, port, icon.bounds, bounds, srcXor );  // plot icon
		
	drawn:
		
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
