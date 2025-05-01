/*
	SystemsTwilightTuneup.cc
	------------------------
	
	System's Twilight Tune-up INIT for Advanced Mac Substitute
	
	Copyright 2022-2025, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
	System's Twilight has various screens with animations that run in the
	background.  Some of them (e.g. open gates) draw concentric circles
	without any buffering -- the circular space is cleared, and then up
	to six circles are drawn via FrameOval().  This results in visible
	flickering, which is not just unsightly, but may pose a usability or
	even a safety issue for users with certain neurological conditions.
	
	DISCLAIMER:  No medical professionals were consulted regarding the
	development of this program.  None of the information contained herein
	constitutes medical advice.  I make no guarantee that this program
	will work as described or make System's Twilight safe for you to use.
	Use at your own risk.
	
	This extension patches the jump table in order to wrap calls to the
	animation function.  By bracketing the animation calls with callouts
	that defer screen updates, we prevent the flickering entirely.
	
	A related issue occurs in the modal dialog for examining or taking
	objects.  Several of them have animated visuals, and one of those
	exhibits flickering as it draws polygons that float around.  (This
	was previously solved by placing all filterProc calls in the scope
	of a raster lock, but this approach interferes with getting live
	feedback during scrolling (as in SFGetFile()) and presumably other
	uses of TrackControl(), and consequently must be reverted.)
	
	This extension patches the polygon animation routine, bracketing
	the drawing between calls to the screen-update-deferring callouts.
	
	A less significant (but still noticeable) visual defect when running
	System's Twilight in Advanced Mac Substitute is the lack of support
	for hierarchical menus.  The You menu is your inventory, containing
	a menu item for each game item you're carrying.  One of those items
	is a memory block, which itself can contain items, represented as a
	submenu.  In AMS, it appears in the menu bar as a regular menu, using
	its provided name (which normally would never be displayed).  While
	the name "intMemInv" is suggestive of its function ("internal memory
	inventory", perhaps), it's quite un-Mac-like to have such cryptic
	coder cant cluttering the user interface.
	
	This extension also patches InsertMenu() to rename the inadvertent
	"intMemInv" menu to the more mainstream "Memory" moniker.
	
	And finally, well into the game, there's one room (The Network Node)
	that elicits a crash upon entry.  A hit-testing function writes a word
	through a pointer, passed to it as an argument, which is always NULL.
	
	This extension patches the hit-testing function to skip the pointer
	write, avoiding the crash in Advanced Mac Substitute.
	
*/

// Mac OS
#ifndef __MENUS__
#include <Menus.h>
#endif
#ifndef __RESOURCES__
#include <Resources.h>
#endif
#ifndef __TEXTUTILS__
#include <TextUtils.h>
#endif
#ifndef __TRAPS__
#include <Traps.h>
#endif

// Standard C
#include <stddef.h>

// mac-glue-utils
#include "mac_glue/Memory.hh"

// mac-sys-utils
#include "mac_sys/load_segment.hh"
#include "mac_sys/trap_address.hh"

// ams-common
#include "callouts.hh"


#pragma exceptions off


Ptr CurrentA5 : 0x0904;

typedef void (*generic_code)();

typedef pascal void (*InsertMenu_ProcPtr)( MenuRef menu, short beforeID );

static generic_code      animator;
static UniversalProcPtr  old_InsertMenu;
static UniversalProcPtr  old_TEInit;

static
void animate_buffered()
{
	lock_screen();
	
	animator();
	
	unlock_screen();
}

static inline
void install_portal_animation_patch()
{
	mac::sys::load_segment( 5 );
	
	const int entry_offset = 1146;
	const Ptr function_loc = CurrentA5 + entry_offset + 2;
	
	generic_code& jump_target = *(generic_code*) function_loc;
	
	animator = jump_target;
	
	jump_target = &animate_buffered;
}

/*
	There's a polygon animation function at CODE 2, offset $0029d2:
	
		void AnimatePretty( DialogRef dialog, short item );
	
	It calls PaintRect() to clear the drawing area and then one of
	PaintPoly(), FillPoly(), or FillCPoly() to draw two polygons.
	In Advanced Mac Substitute, this results in visible flickering.
	
	Wedge in calls to lock_screen() and unlock_screen() surrounding
	the drawing calls.  The first call site is a code segment that
	determines the midpoint of the drawing area.  It calculates
	[(left + right) / 2, (top + bottom) / 2], using DIVS (and thus
	requiring EXT.L to prepare a 32-bit dividend).  However, since
	these are port-local (i.e. window-local) coordinates, we know
	that they're (a) positive integers, so an unsigned division is
	equivalent, and (b) less than 2^15, so a 16-bit operation is
	sufficient.  Thus we use a bit shift and no sign extension,
	saving two words for each dimension, more than plenty for a
	16-bit jump to lock_screen().
	
	The second call site includes a redundant TST instruction
	(immediately following an ADD to the same destination operand).
	While it would work to replace just the TST instruction with the
	new JSR (because unlock_screen() doesn't alter the CCR), it would
	create a /dependency/ on that fact, which is neither necessary nor
	desirable.  It's simple enough to slide down a couple preceding
	instructions and place the unlock_screen() call ahead of them.
*/

static const UInt16 old_ante_pretty[] =
{
	0x48c5,       // 002a34:  EXT.L    D5
	0x8bfc,   2,  // 002a36:  DIVS.W   #0x0002,D5
	0x382e,  -6,  // 002a3a:  MOVE.W   (-6,A6),D4
	0xd86e, -10,  // 002a3e:  ADD.W    (-10,A6),D4
	0x48c4,       // 002a42:  EXT.L    D4
	0x89fc,   2,  // 002a44:  DIVS.W   #0x0002,D4
};

static const UInt16 new_ante_pretty[] =
{
	0xe245,          // 002a34:  ASR.W    #1,D5
	0x4e71,          // 002a36:  NOP
	0x4e71,          // 002a38:  NOP
	0x382e,  -6,     // 002a3a:  MOVE.W   (-6,A6),D4
	0xd86e, -10,     // 002a3e:  ADD.W    (-10,A6),D4
	0xe244,          // 002a42:  ASR.W    #1,D4
	0x4eb8, 0xffec,  // 002a44:  JSR      lock_screen
};

static const UInt16 old_post_pretty[] =
{
	0x1b7c,     1, -8218,  // 002c50:  MOVE.B   #0x01,(-8218,A5)
	0x302d, -8200,         // 002c56:  MOVE.W   (-8200,A5),D0
	0xd16d, -8202,         // 002c5a:  ADD.W    D0,(-8202,A5)
	0x4a6d, -8202,         // 002c5e:  TST.W    (-8202,A5)
};

static const UInt16 new_post_pretty[] =
{
	0x4eb8, 0xffea,         // 002c50:  JSR      unlock_screen
	0x1b7c,      1, -8218,  // 002c54:  MOVE.B   #0x01,(-8218,A5)
	0x302d,  -8200,         // 002c5a:  MOVE.W   (-8200,A5),D0
	0xd16d,  -8202,         // 002c5e:  ADD.W    D0,(-8202,A5)
};

static inline
void install_CODE_2_patch( Handle h )
{
	using mac::glue::GetHandleSize_raw;
	
	const int intro_offset = 0x002a34;
	const int outro_offset = 0x002c50;
	
	if ( h  &&  GetHandleSize_raw( h ) >= outro_offset + sizeof old_post_pretty )
	{
		Ptr p_in  = (*h + intro_offset);
		Ptr p_out = (*h + outro_offset);
		
		if ( fast_memequ( p_in,  old_ante_pretty, sizeof old_ante_pretty )  &&
		     fast_memequ( p_out, old_post_pretty, sizeof old_post_pretty ) )
		{
			fast_memcpy( p_in,  new_ante_pretty, sizeof new_ante_pretty );
			fast_memcpy( p_out, new_post_pretty, sizeof new_post_pretty );
			
			HNoPurge( h );
		}
	}
}

/*
	There's a hit-testing function at CODE 5, offset $000c7a:
	
		short HitTestPortals( Point, short*, Boolean );
	
	It has only one call site (at CODE 3, offset $003554), which always
	passes NULL for the pointer (and false for the flag, incidentally).
	Since the result is discarded anyway, skip the write to avoid a crash.
*/

static inline
void install_CODE_5_patch( Handle h )
{
	using mac::glue::GetHandleSize_raw;
	
	const int query_offset = 0x000cc8;
	
	if ( h  &&  GetHandleSize_raw( h ) >= query_offset + sizeof (UInt32) )
	{
		UInt32* p;
		
		p = (UInt32*) (*h + query_offset);
		
		if ( *p == 0x226E000C )
		{
			/*
				Old:  000cc8:  MOVEA.L  (12,A6),A1         // always NULL
				      000ccc:  MOVE.W   (-3058,A0),(A1)
				      000cd0:  MOVE.W   D7,D0
				      000cd2:  BRA.S    *+12    ; $000cde
				
				New:  000cc8:  MOVE.W   D7,D0
				      000cca:  BRA.S    *+20    ; $000cde
				      000ccc:  ...
				      
			*/
			
			*p = 0x30076012;
			
			HNoPurge( h );
		}
	}
}

static const Byte intMemInv[] = "\p" "intMemInv";
static const Byte Memory[]    = "\p" "Memory";

static
pascal
void InsertMenu_patch( MenuRef menu, short beforeID )
{
	const Byte* title = menu[0]->menuData;
	
	if ( fast_memcmp( title, intMemInv, sizeof intMemInv ) == 0 )
	{
		Munger( (Handle) menu, offsetof( MenuInfo, menuData ),
		        NULL,   sizeof intMemInv,
		        Memory, sizeof Memory );
	}
	
	InsertMenu_ProcPtr InsertMenu = (InsertMenu_ProcPtr) old_InsertMenu;
	
	InsertMenu( menu, beforeID );
}

static inline
void install_InsertMenu_patch()
{
	old_InsertMenu = mac::sys::get_trap_address( _InsertMenu );
	
	mac::sys::set_trap_address( (ProcPtr) InsertMenu_patch, _InsertMenu );
}

static
pascal
void TEInit_patch()
{
	if ( Handle h = Get1Resource( 'STwZ', 0 ) )
	{
		ReleaseResource( h );
		
		install_portal_animation_patch();
		
		install_CODE_2_patch( Get1Resource( 'CODE', 2 ) );
		install_CODE_5_patch( Get1Resource( 'CODE', 5 ) );
		
		install_InsertMenu_patch();
	}
	
	old_TEInit();
}

int main()
{
	old_TEInit = mac::sys::get_trap_address( _TEInit );
	
	mac::sys::set_trap_address( (ProcPtr) TEInit_patch, _TEInit );
	
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
