/*
	VanlandinghamTuneup.cc
	----------------------
	
	Vanlandingham Tune-up INIT for Mac OS
	
	Copyright 2024-2026, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
	Vanlandingham is a bouncing ball demo (like Amiga's "boing!")
	designed to run on Macs with the original graphics hardware.
	
	https://www.v68k.org/ams/demo/Vanlandingham-AMS-demo.mp4
	
	Here's a brief synopsis of the Vanlandingham demo:
	
		1. Clear the screen to white.
		2. Dissolve to a text display.
		3. Display an animated busy cursor while setting up.
		4. Beep, flash the screen, and display a blinking prompt.
		5. Dissolve to a holodeck-like grid space[1].
		6. Display a bouncing, rotating ball (via page flipping).
		7. On keypress, dissolve back to the empty grid space.
		8. Dissolve to a gray checkerboard pattern.
	
	Note that the dissolve in #7 (which is localized to the ball)
	changes a single bit at a time.  The other dissolves (#2, #5,
	and #8) cover the whole screen and set entire bytes at once.
	
	[1] Vanlandingham's grid is derived from the one in "boing!",
	which debuted in January 1984.  Star Trek: The Next Generation
	(in which a holodeck is first shown) didn't air until 1987.
	
	
	This is a set of application hot patches for Vanlandingham.
	Two of them are always applied in Advanced Mac Substitute:
	
	One replaces a 20-word busy loop with a 4-word Delay call.
	Another rewrites the dissolve-bytes routine to use only non-
	volatile registers across a Toolbox trap call to HideCursor().
	
	Although Vanlandingham's "sleep" function is only called
	once at startup (for 10 ticks) and once on Quit (for 12 ticks),
	the patch does measurably reduce CPU time by those amounts
	when run in Advanced Mac Substitute.
	
	HideCursor() is a Toolbox trap call, which is allowed to clobber
	the usual volatile registers: D0-D2 and A0-A1.  In practice,
	the implementation in Advanced Mac Substitute would clobber A0
	had it not been written in assembly language to avoid this very
	outcome.  This patch allows the asm rewrite to be reverted.
	
	(Earlier in the development of Advanced Mac Substitute, the
	HideCursor() implementation did clobber other registers, which
	caused Vanlandingham to crash, necessitating the asm rewrite.
	Now, the only symptom would be drawing garbage to the screen
	on account of the clobbered source address.  Presumably, Mac OS'
	HideCursor() implementation in early ROMs doesn't clobber any of
	the volatile registers (except possibly D0, which Vanlandingham
	doesn't use here), but nothing prevents an INIT from patching it
	with its own code that does.
	
	The remaining patches are only applied in an "expanded graphics"
	environment -- i.e. anything other than the original 512x342x1*2
	screen buffers in the original Macintosh through the Plus, SE,
	and Classic.  These patches may be applied not just in Advanced
	Mac Substitute, but in real Mac OS as well, in which case the
	first two patches are also applied.  (In real Mac OS, those two
	patches are inconsequential enough that it's not worth patching
	anything at all just to have them, nor is it worth the logic to
	skip them when the graphics patches are applied in real Mac OS
	but not Advanced Mac Substitute.)
	
	Vanlandingham predates color Macintosh computers and assumes
	the original graphics environment:  One-bit depth, 512 x 342
	resolution, and a secondary screen buffer 32K below the main
	one (with a bit in the VIA controlling which one is active).
	
	None of that is guaranteed in an expanded graphics environment,
	and most of it doesn't exist there at all -- with one-bit depth
	being the sole exception.  *Everything* that puts pixels on the
	screen must be patched.  We'll also need our own overlay window.
	
	We start by patching our own patch.  The dissolve-bytes routine
	that we rewrote to avoid depending on HideCursor() to preserve
	volatile registers contains a MOVE.B instruction that actually
	copies an eight-pixel byte to the screen.  That instruction is
	replaced with a JSR to DissolveBytes_one_byte() to update the
	real screen.
	
	The PrintWait() function inverts the screen and draws and erases
	text.  Our patch sets the current port to the overlay, calls the
	original PrintWait(), and restores the current port.  Because we
	replaced a two-word relative jump with a three-word absolute one
	(overwriting a HideCursor() trap call), we call it here instead.
	
	SetVBLState() is called to initiate a page flip.  In effect, we
	tail-patch it -- but instead of extrapolating its address as we
	did with PrintWait(), we simply duplicate its code.  But rather
	than jump to the return address, we push it onto the stack and
	jump to Blit(), which copies the pseudo-screen to the real one.
	
	The dissolve_bits routine doesn't have a MOVE.B instruction to
	replace.  It clears or sets a single pixel using BCLR or BSET.
	Our patch deduplicates the loop control logic to make space for
	a JSR to DissolveBits_one_byte(), which updates the real screen.
	
	Finally, we patch the exit code (overwriting some no-op runtime
	boilerplate) to restore MBarHeight, after zeroing it initially.
	
*/

// Mac OS
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// mac-glue-utils
#include "mac_glue/Memory.hh"

// mac-sys-utils
#include "mac_sys/has/virtualization.hh"
#include "mac_sys/trap_address.hh"

// VanlandinghamTuneup
#include "dissolve_bits.hh"
#include "dissolve_bytes.hh"
#include "QDGlobals.hh"
#include "sleep.hh"
#include "transfer.hh"


#pragma exceptions off


#define LENGTH(array)  (sizeof (array) / sizeof (array)[0])

#define VEC_LEN(array)   array, LENGTH (array)
#define VEC_SIZE(array)  array, sizeof (array)


enum
{
	_InitCursor  = 0xA850,
	_HideCursor  = 0xA852,
	_SetPort     = 0xA873,
	_ExitToShell = 0xA9F4,
	_GetAppParms = 0xA9F5,
};

Ptr      ScrnBase   : 0x0824;
GDHandle MainDevice : 0x08A4;
short    MBarHeight : 0x0BAA;

static Boolean payload_enabled;

static UniversalProcPtr old_GetAppParms;
static UniversalProcPtr old_InitCursor;

static QDGlobals qd;

static GrafPtr user_port;

static WindowRef overlay_window;

static BitMap pseudo_screenBits;

static Ptr page_1;
static Ptr page_2;

static inline
bool has_expanded_graphics()
{
	return (UInt32) ScrnBase >= 0x400000;
}

static inline
short width( const Rect& r )
{
	return r.right - r.left;
}

static inline
short height( const Rect& r )
{
	return r.bottom - r.top;
}

static inline
void my_memcpy( void* dst, const void* src, long n )
{
	BlockMoveData( src, dst, n );
}

static inline
bool equal_words( const UInt16* a, const UInt16* b, short n )
{
	do
	{
		if ( *a++ != *b++ )
		{
			return false;
		}
	}
	while ( --n > 0 );
	
	return true;
}

static
asm
void PrintWait_patch( const Byte* clickToBegin )
{
	MOVE.L   overlay_window,-(SP)
	_SetPort
	
	MOVEA.L  (SP)+,A0    // return address, CODE start + $0015ca
	MOVEA.L  (SP)+,A1    // argument
	
	MOVE.L   A0,-(SP)
	MOVE.L   A1,-(SP)
	
	SUBA.W   #0x12cc,A0  // CODE start + $0002fe
	
	JSR      (A0)        // PrintWait, pops argument
	
	MOVE.L   user_port,-(SP)
	_SetPort
	
	_HideCursor
	
	RTS
}

static
void Blit( short state : __D0 )
{
	switch ( state )
	{
		case 0:
		case 1:
			pseudo_screenBits.baseAddr = state ? page_1 : page_2;
			
			SetPort( overlay_window );
			
			StdBits( &pseudo_screenBits,
			         &pseudo_screenBits.bounds,
			         &pseudo_screenBits.bounds,
			         srcCopy,
			         NULL );
			
			SetPort( user_port );
			
			break;
		
		default:
			break;
	}
}

static
pascal
asm
void SetVBLState_patch( void* task, short state )
{
	MOVEA.L  (A7)+,A1    // pop return address
	MOVE.W   (A7)+,D0    // pop next_state
	MOVEA.L  (A7)+,A0    // pop VBL task
	
loop:
	BSET     #0,14(A0)   // claim lock
	BNE.S    loop
	
	MOVE.W   D0,18(A0)   // vbl.state = next_state
	BCLR     #0,14(A0)   // unlock
	
	PEA      (A1)
	JMP      Blit
}

static
asm
void DissolveBytes_one_byte()
{
	MOVE.B   (A2,D5.W),D0        // new byte value
	MOVE.W   D5,D1               // src byte index
	
	JMP      set_8px_at_offset
}

static
asm
void DissolveBits_one_byte()
{
	// dst byte addr in A3
	
	// D1 is free
	// D4 is free
	// D6 is free, but leave the high word cleared
	// D7 is free
	// A3 is free
	
	LINK     A6,#0
	MOVEM.L  D0/D2/A0-A1,-(SP)
	
	MOVE.L   A3,D1
	SUB.L    page_1,D1
	
	MOVE.B   (A3),D0
	JSR      set_8px_at_offset
	
	MOVEM.L  (SP)+,D0/D2/A0-A1
	UNLK     A6
	RTS
}

static inline
void install_sleep_patch( Handle h, Size handle_size )
{
	if ( handle_size >= 0x000104 )
	{
		UInt16* p = (UInt16*) (*h + 0x0000dc);
		
		if ( equal_words( p, VEC_LEN( shipped_sleep_loop ) ) )
		{
			my_memcpy( p, VEC_SIZE( patched_sleep_trap ) );
		}
	}
}

static inline
void install_dissolve_bytes_patch( Handle h, Size handle_size )
{
	if ( handle_size >= 0x0025f2 )
	{
		UInt16* pA = (UInt16*) (*h + 0x002566);
		UInt16* pB = (UInt16*) (*h + 0x0025c4);
		
		bool patch_dissolve_bytes =
			equal_words( pA, VEC_LEN( shipped_dissolve_bytes_A ) )  &&
			equal_words( pB, VEC_LEN( shipped_dissolve_bytes_B ) );
		
		if ( patch_dissolve_bytes )
		{
			my_memcpy( pA, VEC_SIZE( patched_dissolve_bytes_A ) );
			my_memcpy( pB, VEC_SIZE( patched_dissolve_bytes_B ) );
		}
	}
}

static
void install_heavy_patches( Handle h, Size handle_size )
{
	const int required_length = 0x0028ac;
	
	if ( handle_size >= required_length )
	{
		UInt16* p;
		UInt32* q;
		
		p = (UInt16*) (*h + 0x0015c4);
		
		*p++ = 0x4eb9;  // JSR PrintWait_patch
		
		q = (UInt32*) p;
		
		*q = (long) &PrintWait_patch;
		
		
		p = (UInt16*) (*h + 0x0019e8);
		
		*p++ = 0x31FC;  // MOVE.W   #20,MBarHeight
		*p++ = 20;
		*p++ = (short) (long) &MBarHeight;
		
		*p++ = _ExitToShell;
		*p++ = 0x4E71;  // NOP
		
		
		p = (UInt16*) (*h + 0x001a02);
		
		*p++ = 0x4ef9;  // JMP SetVBLState_patch
		
		q = (UInt32*) p;
		
		*q = (long) &SetVBLState_patch;
		
		
		p = (UInt16*) (*h + 0x0025dc);
		
		*p++ = 0x4eb9;  // JSR DissolveBytes_one_byte
		
		q = (UInt32*) p;
		
		*q = (long) &DissolveBytes_one_byte;
		
		
		p = (UInt16*) (*h + 0x002886);
		
		if ( equal_words( p, VEC_LEN( shipped_dissolve_bits ) ) )
		{
			my_memcpy( p, VEC_SIZE( patched_dissolve_bits ) );
			
			q = (UInt32*) (*h + 0x00289a);
			
			*q = (long) &DissolveBits_one_byte;
		}
	}
}

static
void install_screen_buffer()
{
	GetPort( &user_port );
	
	BitMap& screenBits = *get_screenBits();
	
	InitGraf( &qd.thePort );
	
	MBarHeight = 0;
	
	InitWindows();
	
	Ptr screen = NewPtrClear( classic_screen_buffer_size + 0x8000 );
	
	if ( ! screen )
	{
		ExitToShell();
	}
	
	const Rect* r = &screenBits.bounds;
	
	Boolean   vis    = true;
	short     procid = plainDBox;
	WindowRef behind = (WindowRef) -1;
	Boolean   box    = false;
	
	overlay_window = NewWindow( NULL, r, "\p", vis, procid, behind, box, 0 );
	
	if ( ! overlay_window )
	{
		ExitToShell();
	}
	
	SetPortWindowPort( overlay_window );
	
	WindowPeek w = (WindowPeek) overlay_window;
	
	SetEmptyRgn( w->updateRgn );
	
	v_offset = (height( screenBits.bounds ) - classic_screen_height) / 2u;
	h_offset = (width ( screenBits.bounds ) - classic_screen_width ) / 2u;
	
	SetOrigin( -h_offset, -v_offset );
	
	SetPort( user_port );
	
	page_2 = screen;
	page_1 = screen + 0x8000;
	
	screenBits.baseAddr = page_1;
	screenBits.rowBytes = classic_screen_rowBytes;
	
	screenBits.bounds.top  = 0;
	screenBits.bounds.left = 0;
	screenBits.bounds.bottom = classic_screen_height;
	screenBits.bounds.right  = classic_screen_width;
	
	pseudo_screenBits = screenBits;
	
	h_offset_bytes = h_offset >> 3;
}

static
void InitCursor_handler()
{
	if ( payload_enabled )
	{
		payload_enabled = false;
		
		if ( Handle h = Get1Resource( 'CODE', 1 ) )
		{
			Size size = mac::glue::GetHandleSize_raw( h );
			
			install_sleep_patch( h, size );
			
			install_dissolve_bytes_patch( h, size );
			
			if ( has_expanded_graphics() )
			{
				if ( MainDevice )
				{
					UInt16 depth = MainDevice[0]->gdPMap[0]->pixelSize;
					
					if ( depth > 1 )
					{
						return;  // multibit depth currently unsupported
					}
				}
				
				install_heavy_patches( h, size );
				
				install_screen_buffer();
			}
		}
	}
}

static
pascal asm void InitCursor_patch()
{
	LINK     A6,#0
	JSR      InitCursor_handler
	UNLK     A6
	
	MOVEA.L  old_InitCursor,A0
	JMP      (A0)
}

static
void GetAppParms_handler()
{
	if ( Handle h = Get1Resource( 'VANL', 0 ) )
	{
		ReleaseResource( h );
		
		payload_enabled = true;
	}
}

static
pascal asm void GetAppParms_patch( Ptr a, Ptr b, Ptr c )
{
	LINK     A6,#0
	JSR      GetAppParms_handler
	UNLK     A6
	
	MOVEA.L  old_GetAppParms,A0
	JMP      (A0)
}

int main()
{
	if ( has_expanded_graphics()  ||  mac::sys::has_v68k() )
	{
		Handle self = Get1Resource( 'INIT', 0 );
		
		DetachResource( self );
		
		old_GetAppParms = mac::sys::get_trap_address( _GetAppParms );
		old_InitCursor  = mac::sys::get_trap_address( _InitCursor );
		
		mac::sys::set_trap_address( (ProcPtr) GetAppParms_patch, _GetAppParms );
		mac::sys::set_trap_address( (ProcPtr) InitCursor_patch,  _InitCursor  );
	}
	
	return 0;
}

/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
