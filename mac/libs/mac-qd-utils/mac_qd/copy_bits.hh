/*
	copy_bits.hh
	------------
*/

#ifndef MACQD_COPYBITS_HH
#define MACQD_COPYBITS_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// mac-qd-utils
#include "mac_qd/get_BitMap_for_CopyBits.hh"
#include "mac_qd/get_bounds_for_CopyBits.hh"


namespace mac {
namespace qd  {

typedef SInt16 TransferMode;

template < class Src, class Dst >
inline
void copy_bits( const Src&    src,
                const Dst&    dst,
                const Rect&   srcRect,
                const Rect&   dstRect,
                TransferMode  mode    = srcCopy,
                RgnHandle     maskRgn = NULL )
{
	const BitMap* srcBits = get_BitMap_for_CopyBits( src );
	const BitMap* dstBits = get_BitMap_for_CopyBits( dst );
	
	CopyBits( srcBits, dstBits, &srcRect, &dstRect, mode, maskRgn );
}

template < class Src, class Dst >
inline
void copy_bits( const Src&    src,
                const Dst&    dst,
                const Rect&   rect,
                TransferMode  mode    = srcCopy,
                RgnHandle     maskRgn = NULL )
{
	copy_bits( src, dst, rect, rect, mode, maskRgn );
}

template < class Src, class Dst >
inline
void copy_bits( const Src&    src,
                const Dst&    dst,
                TransferMode  mode    = srcCopy,
                RgnHandle     maskRgn = NULL )
{
	copy_bits( src,
	           dst,
	           get_bounds_for_CopyBits( src ),
	           get_bounds_for_CopyBits( dst ),
	           mode,
	           maskRgn );
}

}
}

#endif
