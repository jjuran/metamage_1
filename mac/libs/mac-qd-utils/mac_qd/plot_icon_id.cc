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


namespace mac {
namespace qd  {

const Fixed fixed_72dpi = 72 << 16;

#if ! __LP64__

#if ! OPAQUE_TOOLBOX_STRUCTS

static inline
Boolean IsPortColor( GrafPtr port )
{
	return port->portBits.rowBytes < 0;
}

#endif

short plot_icon_id( const Rect& bounds, short id )
{
	if ( ! TARGET_CPU_68K  ||  mac::sys::has_IconUtilities() )
	{
		return PlotIconID( &bounds, 0, 0, id );
	}
	else if ( Handle h = GetResource( 'ICN#', id ) )
	{
		GrafPtr port = thePort();
		
		Handle icl8;
		
		if ( IsPortColor( port )  &&  (icl8 = GetResource( 'icl8', id )) )
		{
			const short bpp     = 8;
			const short clut_id = bpp;
			
			if ( CTabHandle ctab = GetCTable( clut_id ) )
			{
				UInt32 mask_data[ 32 ];
				
				BlockMoveData( *h + 128, mask_data, sizeof mask_data );
				
				BitMap mask =
				{
					(Ptr) mask_data,
					4,
					{ 0, 0, 32, 32 },
				};
				
				copy_bits( mask, port, mask.bounds, bounds, srcBic );
				
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
		
		PlotIcon( &bounds, h );
		
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
