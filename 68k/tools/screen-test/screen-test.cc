/*
	screen-test.cc
	--------------
*/

// Mac OS
#include <MacMemory.h>
#include <Quickdraw.h>


QDGlobals qd;


static inline asm UInt32 mulu_w( UInt16 a : __D1, UInt16 b : __D0 )
{
	MULU.W  D1,D0
}

static void screen_test()
{
	::InitGraf( &qd.thePort );
	
	GrafPtr port = (GrafPtr) NewPtr( sizeof (GrafPort) );
	
	OpenPort( port );
	
	const BitMap& screenBits = qd.screenBits;
	
	const Rect& bounds = screenBits.bounds;
	
	const short n_rows = bounds.bottom - bounds.top;
	
	const short rowBytes = screenBits.rowBytes;
	
	const uint32_t size = mulu_w( n_rows, rowBytes );
	
	::FillRect( &screenBits.bounds, &qd.gray );
	
	Rect menubar_rect = { 0, 0, 19, 512 };
	
	::EraseRect( &menubar_rect );
	
	Rect menubar_edge_rect = { 19, 0, 20, 512 };
	
	::PaintRect( &menubar_edge_rect );
	
	Ptr p = screenBits.baseAddr;
	
	*p |= 0xF8;  p += rowBytes - 1;  *p++ |= 0x1F;
	*p |= 0xE0;  p += rowBytes - 1;  *p++ |= 0x07;
	*p |= 0xC0;  p += rowBytes - 1;  *p++ |= 0x03;
	*p |= 0x80;  p += rowBytes - 1;  *p++ |= 0x01;
	*p |= 0x80;  p += rowBytes - 1;  *p++ |= 0x01;
	
	p = screenBits.baseAddr + size;
	
	*--p |= 0x1F;  p -= rowBytes - 1;  *p |= 0xF8;
	*--p |= 0x07;  p -= rowBytes - 1;  *p |= 0xE0;
	*--p |= 0x03;  p -= rowBytes - 1;  *p |= 0xC0;
	*--p |= 0x01;  p -= rowBytes - 1;  *p |= 0x80;
	*--p |= 0x01;  p -= rowBytes - 1;  *p |= 0x80;
}

int main( int argc, char** argv )
{
	screen_test();
	
	return 0;
}
