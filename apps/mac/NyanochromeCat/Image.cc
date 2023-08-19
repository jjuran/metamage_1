/*
	Image.cc
	--------
*/

#include "Image.hh"

// mac-cg-utils
#include "mac_cg/images.hh"

// mac-qd-utils
#include "mac_qd/get_pix_rowBytes.hh"


#ifdef __APPLE__

using mac::qd::get_pix_rowBytes;

using nyancat::n_frames;

CGImageRef images[ n_frames ];

void make_images_from_gworld( GWorldPtr gworld )
{
	PixMapHandle pix = GetGWorldPixMap( gworld );
	
	const bool locked = LockPixels( pix );
	
	if ( ! locked )
	{
		ExitToShell();
	}
	
	const PixMap& pixmap = **pix;  // Blocks don't move in OS X
	
	const short width  = pixmap.bounds.right - pixmap.bounds.left;
	const short height = (pixmap.bounds.bottom - pixmap.bounds.top) / n_frames;
	
	const long stride = get_pix_rowBytes( pix );
	
	Ptr baseAddr = pixmap.baseAddr;
	
	size_t size = stride * height;
	
	for ( int i = 0;  i < n_frames;  ++i )
	{
		using mac::cg::create_gray_paint_image;
		
		images[ i ] = create_gray_paint_image( width,
		                                       height,
		                                       1,
		                                       stride,
		                                       baseAddr );
		
		baseAddr += size;
	}
}

#endif  // #ifdef __APPLE__
