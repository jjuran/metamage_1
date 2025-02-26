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
#include "scoped_zone.hh"
#include "screen_lock.hh"

// ams-core
#include "arrow.hh"


#pragma exceptions off


enum
{
	/*
		The presence of this value in CrsrSave at startup
		indicates to ams-core that a hardware cursor exists.
	*/
	
	kHardwareCursor = 0x68647772,  // 'hdwr'
};

typedef UInt32 Buffer[ 16 ];

Ptr   ScrnBase  : 0x0824;
Point Mouse     : 0x0830;
Rect  CrsrPin   : 0x0834;
Rect  CrsrRect  : 0x083C;
Cursor TheCrsr  : 0x0844;
Ptr   CrsrSave  : 0x088C;
short CrsrRow   : 0x08AC;
short DepthLog2 : 0x08AE;
char  CrsrVis   : 0x08CC;
char  CrsrBusy  : 0x08CD;
short CrsrState : 0x08D0;
char CrsrObscure: 0x08D2;

static bool hardware_cursor;

static Ptr    CrsrAddr;
static Buffer bits_under_cursor;

static short cursor_rowBytes;
static short CrsrSave_rowBytes = 4;

static Ptr crsr_face;
static Ptr crsr_mask;


static inline
unsigned bytes_from_px( UInt16 px )
{
	return px >> (3 - DepthLog2);
}

static inline
UInt16 equal_or_lesser_even_byte_boundary( UInt16 px )
{
	const UInt16 mask = 0xF >> DepthLog2;
	
	return px & ~mask;
}

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
	hardware_cursor = (OSType) CrsrSave == kHardwareCursor;
	
	JHideCursor   = &hide_cursor;
	JShowCursor   = &show_cursor;
	JShieldCursor = &shield_cursor;
	JInitCrsr     = &init_cursor;
	JSetCrsr      = &set_cursor;
	JCrsrObscure  = &obscure_cursor;
	
	CrsrSave = (Ptr) bits_under_cursor;
	
	CrsrState = -1;  // Invisible cursor, at first
	
	if ( hardware_cursor )
	{
		notify_cursor_moved( *(long*) &Mouse );
	}
	else if ( DepthLog2 == 3 )
	{
		cursor_rowBytes   = sizeof (UInt16) << DepthLog2;
		CrsrSave_rowBytes = sizeof (UInt32) << DepthLog2;
		
		Size CrsrSave_size = CrsrSave_rowBytes * 16;
		Size crsrtile_size = cursor_rowBytes   * 16;
		
		Size buffer_size = CrsrSave_size + crsrtile_size * 2;
		
		CrsrSave = (scoped_zone(), NewPtr( buffer_size ));
		
		crsr_face = CrsrSave + CrsrSave_size;
		crsr_mask = CrsrSave + CrsrSave_size + crsrtile_size;
	}
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
	
	short left  = equal_or_lesser_even_byte_boundary( h );
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
	
	CrsrAddr = ScrnBase + top * CrsrRow + bytes_from_px( left );
	
	return;
	
empty:
	set_empty_rect( &CrsrRect );
	CrsrAddr = NULL;
}

static inline
void save_bits_under_cursor( short n )
{
	const short rowBytes = CrsrSave_rowBytes;
	
	blit_bytes( CrsrAddr, CrsrRow, CrsrSave, rowBytes, rowBytes, n );
}

static inline
void restore_bits_under_cursor( short n )
{
	const short rowBytes = CrsrSave_rowBytes;
	
	blit_bytes( CrsrSave, rowBytes, CrsrAddr, CrsrRow, rowBytes, n );
}

static inline
void plot_cursor( Ptr addr, short shift, short h_trim, short v_skip, short n )
{
	if ( crsr_face )
	{
		screen_lock lock;
		
		const short row_n = cursor_rowBytes;
		
		blit_bytes( crsr_mask, row_n, CrsrAddr, CrsrRow, row_n, n, srcBic );
		blit_bytes( crsr_face, row_n, CrsrAddr, CrsrRow, row_n, n, srcXor );
		
		return;
	}
	
	plot_cursor( &TheCrsr, addr, shift, h_trim, v_skip, n, CrsrRow );
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
		CrsrVis = false;
		
		if ( hardware_cursor )
		{
			notify_cursor_vis( false );
			return;
		}
		
		--CrsrBusy;
		
		erase_cursor();
		
		set_empty_rect( &CrsrRect );
		CrsrAddr = NULL;
		
		++CrsrBusy;
	}
}

void show_cursor()
{
	if ( CrsrState >= 0  ||  ++CrsrState < 0  ||  CrsrVis  ||  CrsrObscure )
	{
		return;
	}
	
	if ( hardware_cursor )
	{
		CrsrVis = true;
		
		notify_cursor_vis( true );
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
	if ( lock_cursor() )
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
		if ( ! hardware_cursor  &&  CrsrState == 0 )
		{
			/*
				Normal cursor operation.
			*/
			
			update_cursor();
		}
		else if ( ! hardware_cursor  &&  lock_cursor() )
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
	
	if ( hardware_cursor )
	{
		notify_cursor_moved( *(long*) &Mouse );
	}
}

void init_cursor()
{
	set_cursor( &arrow );
	
	CrsrObscure = false;
	
	if ( CrsrState < 0 )
	{
		CrsrState = -1;
		
		show_cursor();
	}
}

static inline
void transcode_8x_1bpp_to_8bpp( const void* src, void* dst, int n )
{
	transcode_8x_1bpp_to_8bpp( src, dst, n, 0x00, 0xFF );
}

pascal void set_cursor( const Cursor* crsr )
{
	fast_memcpy( &TheCrsr, crsr, sizeof (Cursor) );
	
	if ( hardware_cursor )
	{
		notify_cursor_set( crsr );
		return;
	}
	
	if ( crsr_face )
	{
		const Size size = sizeof (UInt16) * 16;
		
		transcode_8x_1bpp_to_8bpp( TheCrsr.data, crsr_face, size );
		transcode_8x_1bpp_to_8bpp( TheCrsr.mask, crsr_mask, size );
	}
	
	if ( CrsrVis )
	{
		update_cursor();
	}
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
