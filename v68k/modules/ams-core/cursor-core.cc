/*
	cursor-core.cc
	--------------
*/

#include "cursor-core.hh"

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// ams-common
#include "callouts.hh"
#include "cursor_jump.hh"
#include "screen_lock.hh"


typedef uint32_t Buffer[ 16 ];

short ScreenRow : 0x0106;
Ptr   ScrnBase  : 0x0824;
Point Mouse     : 0x0830;
Rect  CrsrPin   : 0x0834;
Rect  CrsrRect  : 0x083C;
Cursor TheCrsr  : 0x0844;
char  CrsrVis   : 0x08CC;
char  CrsrBusy  : 0x08CD;

static Ptr    CrsrAddr;
static Buffer CrsrSave;
static short  CrsrState = -1;  // Invisible cursor, at first


static inline
asm char lock_cursor()
{
	TAS.B    CrsrBusy
	SEQ.B    D0
}

static inline
void unlock_cursor()
{
	CrsrBusy = 0;
}

static inline
asm
void set_empty_rect( Rect* r : __A0 )
{
	CLR.L    (A0)+
	CLR.L    (A0)
}

void init_lowmem_Cursor()
{
	JHideCursor = &hide_cursor;
	JShowCursor = &show_cursor;
	JInitCrsr   = &init_cursor;
	JSetCrsr    = &set_cursor;
}

static
void set_Crsr_vars( short h, short v )
{
	h -= TheCrsr.hotSpot.h;
	v -= TheCrsr.hotSpot.v;
	
	short top    = v;
	short bottom = v + 16;
	
	if ( top < 0 )
	{
		if ( bottom <= 0 )
		{
			goto empty;
		}
		
		top = 0;
	}
	else if ( bottom > CrsrPin.bottom )
	{
		if ( top >= CrsrPin.bottom )
		{
			goto empty;
		}
		
		bottom = CrsrPin.bottom;
	}
	
	short left  = h & ~0xF;
	short right = left + 32;
	
	if ( left < 0 )
	{
		if ( right <= 0 )
		{
			goto empty;
		}
		
		left  = 0;
		right = 32;
	}
	else if ( right > CrsrPin.right )
	{
		if ( left >= CrsrPin.right )
		{
			goto empty;
		}
		
		right = CrsrPin.right;
		left  = right - 32;
	}
	
	CrsrRect.top    = top;
	CrsrRect.left   = left;
	CrsrRect.bottom = bottom;
	CrsrRect.right  = right;
	
	CrsrAddr = ScrnBase + top * ScreenRow + (left >> 3);
	
	return;
	
empty:
	set_empty_rect( &CrsrRect );
	CrsrAddr = NULL;
}

static
void save_bits_under_cursor( short n )
{
	Ptr addr = CrsrAddr;
	
	uint32_t* p = CrsrSave;
	
	while ( --n >= 0 )
	{
		*p++ = *(uint32_t*) addr;
		
		addr += ScreenRow;
	}
}

static
void restore_bits_under_cursor( short n )
{
	Ptr addr = CrsrAddr;
	
	uint32_t* p = CrsrSave;
	
	while ( --n >= 0 )
	{
		*(uint32_t*) addr = *p++;
		
		addr += ScreenRow;
	}
}

static
void plot_cursor( Ptr addr, short shift, short h_trim, short v_skip, short n )
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
		
		addr += ScreenRow;
	}
}

static
void erase_cursor()
{
	screen_lock lock;
	
	const short n_rows = CrsrRect.bottom - CrsrRect.top;
	
	restore_bits_under_cursor( n_rows );
}

static
void paint_cursor( short h, short v )
{
	set_Crsr_vars( h, v );
	
	if ( CrsrAddr == NULL )
	{
		return;
	}
	
	screen_lock lock;
	
	short h_trim = 0;
	short v_skip = 0;
	short v_count = CrsrRect.bottom - CrsrRect.top;
	
	h -= TheCrsr.hotSpot.h;
	v -= TheCrsr.hotSpot.v;
	
	if ( v < 0 )
	{
		v_skip = -v;
	}
	
	Ptr plotAddr = CrsrAddr;
	
	if ( h < 0 )
	{
		h_trim = -1;
	}
	else if ( h >= CrsrPin.right - 16 )
	{
		h_trim = 1;
		
		plotAddr += 2;
	}
	
	save_bits_under_cursor( v_count );
	plot_cursor           ( plotAddr, h & 0xF, h_trim, v_skip, v_count );
}

void hide_cursor()
{
	if ( CrsrState-- == 0 )
	{
		--CrsrBusy;
		
		erase_cursor();
		
		set_empty_rect( &CrsrRect );
		CrsrAddr = NULL;
		CrsrVis = false;
		
		++CrsrBusy;
	}
}

void show_cursor()
{
	if ( CrsrState >= 0 )
	{
		return;
	}
	
	--CrsrBusy;
	
	if ( ++CrsrState >= 0 )
	{
		CrsrVis = true;
		
		paint_cursor( Mouse.h, Mouse.v );
	}
	
	++CrsrBusy;
}

void update_cursor_location()
{
	if ( CrsrState == 0  &&  lock_cursor() )
	{
		screen_lock lock;
		
		erase_cursor();
		paint_cursor( Mouse.h, Mouse.v );
		
		unlock_cursor();
	}
}

void init_cursor()
{
	if ( CrsrState < 0 )
	{
		CrsrState = -1;
		
		show_cursor();
	}
}

pascal void set_cursor( const Cursor* crsr )
{
	fast_memcpy( &TheCrsr, crsr, sizeof (Cursor) );
	
	update_cursor_location();
}
