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


#pragma exceptions off


typedef uint32_t* Saved;
typedef uint32_t Buffer[ 16 ];

short ScreenRow : 0x0106;
Ptr   ScrnBase  : 0x0824;
Point Mouse     : 0x0830;
Rect  CrsrPin   : 0x0834;
Rect  CrsrRect  : 0x083C;
Cursor TheCrsr  : 0x0844;
Saved CrsrSave  : 0x088C;
char  CrsrVis   : 0x08CC;
char  CrsrBusy  : 0x08CD;
short CrsrState : 0x08D0;
char CrsrObscure: 0x08D2;

static Ptr    CrsrAddr;
static Buffer bits_under_cursor;


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
	JHideCursor   = &hide_cursor;
	JShowCursor   = &show_cursor;
	JShieldCursor = &shield_cursor;
	JInitCrsr     = &init_cursor;
	JSetCrsr      = &set_cursor;
	JCrsrObscure  = &obscure_cursor;
	
	CrsrSave = bits_under_cursor;
	
	CrsrState = -1;  // Invisible cursor, at first
}

static inline
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

static inline
void save_bits_under_cursor( short n )
{
	Ptr addr = CrsrAddr;
	
	uint32_t* p = bits_under_cursor;
	
	while ( --n >= 0 )
	{
		*p++ = *(uint32_t*) addr;
		
		addr += ScreenRow;
	}
}

static inline
void restore_bits_under_cursor( short n )
{
	Ptr addr = CrsrAddr;
	
	uint32_t* p = bits_under_cursor;
	
	while ( --n >= 0 )
	{
		*(uint32_t*) addr = *p++;
		
		addr += ScreenRow;
	}
}

static inline
void plot_cursor( Ptr addr, short shift, short h_trim, short v_skip, short n )
{
	screen_lock lock;
	
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
	
	short h_trim = 0;
	short v_skip = 0;
	short v_count = CrsrRect.bottom - CrsrRect.top;
	
	save_bits_under_cursor( v_count );
	
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
	
	plot_cursor( plotAddr, h & 0xF, h_trim, v_skip, v_count );
}

void hide_cursor()
{
	--CrsrState;
	
	if ( CrsrVis )
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
	if ( CrsrState >= 0  ||  ++CrsrState < 0  ||  CrsrVis  ||  CrsrObscure )
	{
		return;
	}
	
	--CrsrBusy;
	
	CrsrVis = true;
	
	paint_cursor( Mouse.h, Mouse.v );
	
	++CrsrBusy;
}

pascal void shield_cursor( const Rect* r, Point pt )
{
	/*
		Shield the cursor from direct screen access.
		
		If the area of screen access might intersect the cursor image,
		hide the cursor.  If not, decrement the cursor level anyway,
		to match the eventual corresponding ShowCursor() call.
	*/
	
	Rect shielded = *r;
	
	OffsetRect( &shielded, -pt.h, -pt.v );
	
	if ( SectRect( &CrsrRect, &shielded, &shielded ) )
	{
		hide_cursor();
	}
	else
	{
		--CrsrState;
	}
}

static
void update_cursor()
{
	if ( CrsrVis  &&  lock_cursor() )
	{
		screen_lock lock;
		
		erase_cursor();
		paint_cursor( Mouse.h, Mouse.v );
		
		unlock_cursor();
	}
}

void update_cursor_location()
{
	CrsrObscure = false;
	
	if ( CrsrVis )
	{
		if ( CrsrState == 0 )
		{
			/*
				Normal cursor operation.
			*/
			
			update_cursor();
		}
		else if ( lock_cursor() )
		{
			/*
				The cursor is visible even with a negative cursor level,
				due to a ShieldCursor() call whose rect didn't intersect
				CrsrRect, leaving the cursor exempt from shielding.
				
				Mouse movement ends that exemption.
			*/
			
			hide_cursor();
			
			++CrsrState;
			
			unlock_cursor();
		}
	}
	else if ( CrsrState == 0  &&  lock_cursor() )
	{
		/*
			The cursor is invisible even with a zero cursor level,
			due to a prior call to ObscureCursor().
			
			Mouse movement makes an obscured cursor visible again.
		*/
		
		--CrsrState;
		
		show_cursor();
		
		unlock_cursor();
	}
}

void init_cursor()
{
	CrsrObscure = false;
	
	if ( CrsrState < 0 )
	{
		CrsrState = -1;
		
		show_cursor();
	}
}

pascal void set_cursor( const Cursor* crsr )
{
	fast_memcpy( &TheCrsr, crsr, sizeof (Cursor) );
	
	update_cursor();
}

void obscure_cursor()
{
	/*
		Obscure the cursor (usually while text is being entered).
		
		Make the cursor invisible, without decrementing the cursor level.
	*/
	
	CrsrObscure = true;
	
	if ( CrsrVis )
	{
		hide_cursor();
		
		++CrsrState;
	}
}
