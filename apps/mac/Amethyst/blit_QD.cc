/*
	blit_QD.cc
	----------
*/

#include "blit_QD.hh"

// amicus
#include "amicus/quickdraw.hh"


static CGrafPtr captured_display_port;

static BitMap srcBits;
static Rect   dstRect;

#if CONFIG_QUICKDRAW

QD_blitter::QD_blitter( CGDirectDisplayID id, CGRect bounds )
{
	dstRect.top    = (short)  bounds.origin.y;
	dstRect.left   = (short)  bounds.origin.x;
	dstRect.bottom = (short) (bounds.origin.y + bounds.size.height);
	dstRect.right  = (short) (bounds.origin.x + bounds.size.width);
	
	// UInt32 cast required for 10.4 PPC, at least
	
	captured_display_port = CreateNewPortForCGDisplayID( (UInt32) id );
}

QD_blitter::~QD_blitter()
{
	Rect bounds;
	PaintRect( GetPortBounds( captured_display_port, &bounds ) );
	
	DisposePort( captured_display_port );
}

void QD_blitter::prep( int stride, int width, int height )
{
	srcBits.rowBytes      = stride;
	srcBits.bounds.bottom = height;
	srcBits.bounds.right  = width;
}

void QD_blitter::blit( const void* src_addr )
{
	srcBits.baseAddr = (Ptr) src_addr;
	
	CopyBits( &srcBits,
	          GetPortBitMapForCopyBits( captured_display_port ),
	          &srcBits.bounds,
	          &dstRect,
	          srcCopy,
	          NULL );
}

#endif
