/*
	blit_CG.cc
	----------
*/

#include "blit_CG.hh"

// mac-cg-utils
#include "mac_cg/colorspaces.hh"
#include "mac_cg/images.hh"


static CGRect display_bounds;
static CGRect image_bounds;

static CGContextRef captured_display_context;

static int w, h, rowBytes, bpp;

static CGColorSpaceRef palette;

CG_blitter::CG_blitter( CGDirectDisplayID id )
{
	display_bounds = CGDisplayBounds( id );
	
#ifdef MAC_OS_X_VERSION_10_3
	
	captured_display_context = CGDisplayGetDrawingContext( id );
	
#endif
}

CG_blitter::~CG_blitter()
{
	/*
		We need both of these to clear the screen before releasing the capture.
		CGContextFlush() doesn't help.
	*/
	
	CGContextFillRect ( captured_display_context, display_bounds );
	CGContextClearRect( captured_display_context, display_bounds );
}

void CG_blitter::set_palette( const unsigned short* colors, int n )
{
	using mac::cg::create_RGB_palette;
	
	CGColorSpaceRelease( palette );  // ok with NULL
	
	palette = create_RGB_palette( colors, n );
}

void CG_blitter::prep( int stride, int width, int height, int depth )
{
	w        = width;
	h        = height;
	bpp      = depth;
	rowBytes = stride;
}

void CG_blitter::area( CGRect bounds )
{
	CGContextFillRect( captured_display_context, display_bounds );
	
	image_bounds = bounds;
}

void CG_blitter::blit( const void* src_addr )
{
	using mac::cg::create_inverted_grayscale;
	using mac::cg::create_simple_image;
	using mac::cg::create_xRGB_1555_BE_image;
	using mac::cg::create_xRGB_8888_BE_image;
	
	void* p = const_cast< void* >( src_addr );
	
	CGImageRef image = NULL;
	
	if ( bpp <= 8 )
	{
		if ( ! palette )
		{
			int n = 1 << bpp;
			
			palette = create_inverted_grayscale( n );
		}
		
		image = create_simple_image( w, h, bpp, rowBytes, palette, p );
	}
	else
	{
		image = bpp == 16 ? create_xRGB_1555_BE_image( w, h, rowBytes, p )
		                  : create_xRGB_8888_BE_image( w, h, rowBytes, p );
	}
	
	if ( image )
	{
		CGContextDrawImage( captured_display_context, image_bounds, image );
		
		CGImageRelease( image );
	}
}
