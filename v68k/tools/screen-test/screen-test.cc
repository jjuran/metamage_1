/*
	screen-test.cc
	--------------
*/

// Mac OS
#include <Quickdraw.h>


QDGlobals qd;

static GrafPtr* thePort_ptr;


static void screen_test()
{
	asm
	{
		LEA  thePort_ptr,A5
	}
	
	::InitGraf( &qd.thePort );
	
	const BitMap& screenBits = qd.screenBits;
	
	const Rect& bounds = screenBits.bounds;
	
	const unsigned n_rows = bounds.bottom - bounds.top;
	
	const unsigned rowBytes = screenBits.rowBytes;
	
	const uint32_t size = n_rows * rowBytes;
	
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
