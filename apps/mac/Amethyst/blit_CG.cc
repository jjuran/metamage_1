/*
	blit_CG.cc
	----------
*/

#include "blit_CG.hh"

// Amethyst
#include "image.hh"


static CGRect display_bounds;
static CGRect image_bounds;

static CGContextRef captured_display_context;

static int w, h, rowBytes, bpp;

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
	const void* p = src_addr;
	
	if ( CGImageRef image = create_monochrome_image( p, rowBytes, w, h, bpp ) )
	{
		CGContextDrawImage( captured_display_context, image_bounds, image );
		
		CGImageRelease( image );
	}
}
