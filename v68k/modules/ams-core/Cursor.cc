/*
	Cursor.cc
	---------
*/

#include "Cursor.hh"

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// ams-core
#include "QDGlobals.hh"
#include "cursor_jump.hh"
#include "screen_lock.hh"


typedef uint32_t Buffer[ 16 ];

Point Mouse : 0x0830;

static Cursor TheCrsr;
static Ptr    CrsrAddr;
static Buffer CrsrSave;
static short  CrsrState = -1;  // Invisible cursor, at first


void init_lowmem_Cursor()
{
	JHideCursor = &hide_cursor;
	JShowCursor = &show_cursor;
}

static
void save_bits_under_cursor( Ptr addr, short rowBytes )
{
	uint32_t* p = CrsrSave;
	
	for ( short i = 0;  i < 16;  ++i )
	{
		*p++ = *(uint32_t*) addr;
		
		addr += rowBytes;
	}
}

static
void restore_bits_under_cursor( Ptr addr, short rowBytes )
{
	uint32_t* p = CrsrSave;
	
	for ( short i = 0;  i < 16;  ++i )
	{
		*(uint32_t*) addr = *p++;
		
		addr += rowBytes;
	}
}

static
void plot_cursor( Ptr addr, short rowBytes, short shift, short h_trim, short v_skip, short n )
{
	const uint16_t* p = (const uint16_t*) TheCrsr.data + v_skip;
	const uint16_t* m = (const uint16_t*) TheCrsr.mask + v_skip;
	
	for ( short i = 0;  i < n;  ++i )
	{
		uint16_t data = *p++;
		uint16_t mask = *m++;
		
		uint16_t* q = (uint16_t*) addr;
		
		if ( h_trim >= 0 )
		{
			*q   &= ~(mask >> shift);
			*q++ ^=   data >> shift;
		}
		
		if ( shift  &&  h_trim <= 0 )
		{
			*q &= ~(mask << (16 - shift));
			*q ^=   data << (16 - shift);
		}
		
		addr += rowBytes;
	}
}

static
void erase_cursor()
{
	screen_lock lock;
	
	QDGlobals& qd = get_QDGlobals();
	
	const short rowBytes = qd.screenBits.rowBytes;
	
	restore_bits_under_cursor( CrsrAddr, rowBytes );
}

static
void paint_cursor( short h, short v )
{
	screen_lock lock;
	
	QDGlobals& qd = get_QDGlobals();
	
	short h_trim = 0;
	short v_skip = 0;
	short v_count = 16;
	
	h -= TheCrsr.hotSpot.h;
	v -= TheCrsr.hotSpot.v;
	
	if ( v < 0 )
	{
		if ( v <= -16 )
		{
			return;
		}
		
		v_skip = -v;
		
		v_count -= v_skip;
		
		v = 0;
	}
	
	const Ptr   baseAddr = qd.screenBits.baseAddr;
	const short rowBytes = qd.screenBits.rowBytes;
	
	Ptr plotAddr = baseAddr + v * rowBytes + (h >> 4) * 2;
	
	CrsrAddr = plotAddr;
	
	if ( v > qd.screenBits.bounds.bottom - 16 )
	{
		v_count = qd.screenBits.bounds.bottom - v;
		
		if ( v_count <= 0 )
		{
			return;
		}
		
		v = qd.screenBits.bounds.bottom - 16;
		
		CrsrAddr = baseAddr + v * rowBytes + (h >> 4) * 2;
	}
	
	if ( h < 0 )
	{
		h_trim = -1;
		
		CrsrAddr += 2;
		plotAddr += 2;
	}
	else if ( h > qd.screenBits.bounds.right - 16 )
	{
		h_trim = 1;
		
		CrsrAddr -= 2;
	}
	
	save_bits_under_cursor( CrsrAddr, rowBytes );
	plot_cursor           ( plotAddr, rowBytes, h & 0xF, h_trim, v_skip, v_count );
}

void hide_cursor()
{
	if ( CrsrState-- == 0 )
	{
		erase_cursor();
	}
}

void show_cursor()
{
	if ( ++CrsrState >= 0 )
	{
		CrsrState = 0;
		
		paint_cursor( Mouse.h, Mouse.v );
	}
}

void update_cursor_location()
{
	if ( CrsrState == 0 )
	{
		screen_lock lock;
		
		erase_cursor();
		paint_cursor( Mouse.h, Mouse.v );
	}
}

pascal void InitCursor_patch()
{
	QDGlobals& qd = get_QDGlobals();
	
	TheCrsr = qd.arrow;
	
	screen_lock lock;
	
	hide_cursor();
	
	CrsrState = 0;
	
	paint_cursor( Mouse.h, Mouse.v );
}

pascal void SetCursor_patch( const Cursor* crsr )
{
	TheCrsr = *crsr;
	
	update_cursor_location();
}

pascal void HideCursor_patch()
{
	hide_cursor();
}

pascal void ShowCursor_patch()
{
	show_cursor();
}
