/*
	blit_QD.cc
	----------
*/

#include "blit_QD.hh"

// mac-qd-utils
#if ! __LP64__
#include "mac_qd/copy_bits.hh"
#endif

// amicus
#include "amicus/quickdraw.hh"


static CGrafPtr captured_display_port;

static PixMap srcBits;
static Rect   dstRect;

#if CONFIG_QUICKDRAW

QD_blitter::QD_blitter( CGDirectDisplayID id )
{
	// UInt32 cast required for 10.4 PPC, at least
	
	captured_display_port = CreateNewPortForCGDisplayID( (UInt32) id );
}

QD_blitter::~QD_blitter()
{
	Rect bounds;
	PaintRect( GetPortBounds( captured_display_port, &bounds ) );
	
	DisposePort( captured_display_port );
}

void QD_blitter::prep( int stride, int width, int height, int depth )
{
	if ( depth > 1 )
	{
		srcBits.hRes =
		srcBits.vRes = 72 << 16;
		
		srcBits.pixelSize = depth;
		srcBits.cmpCount  = 1;
		srcBits.cmpSize   = depth;
		
		stride |= 0x8000;
	}
	
	srcBits.rowBytes      = stride;
	srcBits.bounds.bottom = height;
	srcBits.bounds.right  = width;
}

void QD_blitter::area( CGRect bounds )
{
	dstRect.top    = (short)  bounds.origin.y;
	dstRect.left   = (short)  bounds.origin.x;
	dstRect.bottom = (short) (bounds.origin.y + bounds.size.height);
	dstRect.right  = (short) (bounds.origin.x + bounds.size.width);
}

void QD_blitter::blit( const void* src_addr )
{
	srcBits.baseAddr = (Ptr) src_addr;
	
	mac::qd::copy_bits( srcBits,
	                    captured_display_port,
	                    srcBits.bounds,
	                    dstRect );
}

#endif
