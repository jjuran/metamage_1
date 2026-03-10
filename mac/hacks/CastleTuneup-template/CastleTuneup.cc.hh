/*
	CastleTuneup.cc.hh
	------------------
	
	Dark Castle Tune-up INIT template for Advanced Mac Substitute
	
	Copyright 2020-2026, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
	This is a set of application hot patches for Dark Castle
	and Beyond Dark Castle.  The two games share a great deal
	of code (with similar sets of compatibility issues with
	Advanced Mac Substitute), though in different locations.
	
	Beyond Dark Castle also introduces new logic that's not
	present in Dark Castle, to support its new features like
	saved games.  This is the subject of our first patch.
	
	Beyond Dark Castle shipped on two 800K disks.  The second
	disk is required for all gameplay.  In order to support
	single-drive systems, Beyond Dark Castle was designed to
	require only the second disk until gameplay ended.  The
	BDC Data B file contains 'CODE' and other resources for
	this purpose, and the saved game resources (type 'MagT')
	are added to BDC Data B, not BDC Prefs (on disk one).
	
	Advanced Mac Substitute maintains a distinction between
	application files and user files.  The Castle Prefs INITs
	already address the problem of Castle Prefs (and BDC Prefs)
	being load-bearing game files (that must exist and aren't
	created by the game) by copying them to the Preferences
	folder if they don't already exist there.  Clearly, saved
	games should also be stored in the user's BDC Prefs file.
	
	Normally we'd patch the relevant 'CODE' resource in our
	startup patch (of _TEInit), but here, we can't do that --
	the code to be patched lives in BDC Data B, which isn't
	opened until gameplay starts.  Instead, we'll patch one
	of the Toolbox routines, _CurResFile.  The game-saving
	logic uses the usual Cur/Use/Add/Use pattern to add the
	resource to a specific resource file.  Presumably, this
	was originally intended to be the prefs file but had to
	be changed to the Data B file to avoid requiring disk
	switches.  In our _CurResFile patch, we try to load the
	'CODE' id=12 resource.  If we're post-game saving a high
	score, BDC Data B will be closed and the load will fail.
	But if we're in-game saving the game state, the load is
	guaranteed to succeed (because the very same resource is
	the one currently executing).  All we need to do here is
	to substitute the BDC Prefs refnum for the Data B refnum.
	
	Since we're patching _CurResFile in the application (and
	not at system startup time), we have to remove the patch
	as well, so we patch _ExitToShell and remove both there.
	
	Another issue in Beyond Dark Castle is that the animated
	fire on the menu screen is accomplished by means of many
	individual writes to the screen buffer.  By patching the
	routine that animates the fireplace and the two sconces,
	we insert a screen lock that limits refreshes to 12 fps.
	
	The subsequent comments refer to "Dark Castle", but apply
	equally to both Dark Castle and Beyond Dark Castle.
	
	As part of its environment check at launch, Data A peeks into
	I/O space at $f80030 for the string "DEADFACEBEEFCAFE".  In
	practice, whether that string exists at that location or not
	is irrelevant, because it only matters if Data A is launched
	with a non-negative CurPageOption -- but Dark Castle doesn't
	do that, at least not in configurations this author is aware
	of.  In any case, the presence of the string presumably signals
	some kind of debugging environment which doesn't apply to real
	Macs nor to Advanced Mac Substitute.
	
	What does apply to Advanced Mac Substitute is access to unmapped
	memory regions, of which this signature check definitely is one.
	This patch replaces the logic with an RTS, preventing the access.
	
	Dark Castle has exceptionally high CPU usage in the title
	screen (~80%) and during gameplay (100%) due to spinlooping.
	The spinloop has an inner loop that executes 6 NOPs and
	increments a counter that is never read anywhere else.
	
	To mitigate this, we patch the outer loop branch offset to
	return directly to the preceding TST.B instruction, skipping
	the inner loop.  The resulting code matches a pattern that
	xv68k recognizes and further patches into a custom A-trap
	invocation.  The trap handler uses the event reactor to wait
	for the loop exit condition without dominating the CPU.
	
	Dark Castle uses a bare bones blitter routine that copies
	bitmaps directly to the screen buffer, bypassing Mac OS
	entirely (which allows it to be used at interrupt time).
	It copies a source, e.g. "foobarbaz", to the destination
	as "foo  bar  baz" (i.e. adjusting for the screen width).
	Dark Castle uses this in-game only.  Beyond Dark Castle
	also uses it for its splash screen, and additionally, for
	the menu screen's fireplace animation, but *without page
	flipping* -- resulting in multiple update notifications
	per scanline (which utterly swamped the front ends using
	Cocoa, prompting the hasty addition of a 500us sleep as
	a mitigation to make them usable for Beyond Dark Castle).
	
	The fill_bytes() callout was originally intended to be
	used for Color QuickDraw support, but it's exactly what
	we need here, and we reimplement the blitter to use it.
	
*/

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// mac-glue-utils
#include "mac_glue/Memory.hh"

// mac-sys-utils
#include "mac_sys/has/virtualization.hh"
#include "mac_sys/trap_address.hh"


enum
{
	lock_screen   = 0xFFFFFFEC,
	unlock_screen = 0xFFFFFFEA,
	
	fast_memcmp = 0xFFFFFFC6,
	fill_bytes  = 0xFFFFFFB2,
};

enum
{
	kOSEventWaiting = 0x0200,
};

enum
{
	_OSEventAvail     = 0xA030,
	_WaitOSEventAvail = 0xA030 | kOSEventWaiting,
	
	_CurResFile  = 0xA994,
	_ExitToShell = 0xA9F4,
	_TEInit      = 0xA9CC,
};

static UniversalProcPtr old_CurResFile;
static UniversalProcPtr old_ExitToShell;
static UniversalProcPtr old_TEInit;


static
void CurResFile_handler()
{
	using mac::glue::GetHandleSize_raw;
	
	enum
	{
		// These are offsets relative to the start of the 'CODE' resource.
		
		offset_to_CurResFile = 0x02d0,
		offset_to_MOVE_pop   = 0x02d2,  // MOVE.W  (A7)+,D6
		offset_to_MOVE_push  = 0x02d4,  // MOVE.W  (-746,A5),-(A7)
		offset_to_A5_index   = offset_to_MOVE_push + 2,
		offset_to_UseResFile = offset_to_A5_index + 2,
		minimum_handle_size  = offset_to_UseResFile + 2,
	};
	
	enum
	{
		A5_index_of_DataB_refnum = (UInt16) -746,
		A5_index_of_Prefs_refnum = (UInt16) -744,
	};
	
#ifdef SAVEGAME_CODE_RESID
	
	if ( Handle h = GetResource( 'CODE', SAVEGAME_CODE_RESID ) )
	{
		Size size = GetHandleSize_raw( h );
		
		if ( size >= minimum_handle_size )
		{
			UInt16* p = (UInt16*) (*h + offset_to_CurResFile);
			
			if ( *p++ == _CurResFile  &&
			     *p++ == 0x3C1F       &&
			     *p++ == 0x3F2D       &&
			     *p   == A5_index_of_DataB_refnum )
			{
				*p = A5_index_of_Prefs_refnum;
			}
		}
	}
	
#endif
}

static
asm
pascal short CurResFile_patch()
{
	LINK     A6,#0
	MOVEM.L  D1-D2/A1,-(SP)
	
	JSR      CurResFile_handler
	
	MOVEM.L  (SP)+,D1-D2/A1
	UNLK     A6
	
	MOVEA.L  old_CurResFile,A0
	JMP      (A0)
}

static
void ExitToShell_patch()
{
	using mac::sys::set_trap_address;
	
	set_trap_address( old_CurResFile,  _CurResFile  );
	set_trap_address( old_ExitToShell, _ExitToShell );
	
	asm
	{
		DC.W     _ExitToShell | 0x400  // autoPop
	}
}

static inline
void install_envcheck_patch( Handle h, Size handle_size )
{
	Ptr p = *h;
	Ptr q = *h + handle_size - sizeof (UInt32);
	
	while ( p <= q )
	{
		/*
			Old:
				MOVEA.L  D3,A1
				LEA      ...,A0
				MOVEM.L  (A0),D0-D3             // DEAD FACE BEEF CAFE
				MOVEA.L  #0x00f80030,A0
				...
			
			New:
				RTS
		*/
		
		if ( *(UInt32*) p == 0x00f80030 )
		{
			p -= 12;
			
			*(UInt16*) p = 0x4E75;  // RTS
			
			HNoPurge( h );
			
			break;
		}
		
		p += 2;
	}
}

static inline
void install_spinloop_patch( Handle h, Size handle_size )
{
	enum
	{
		// These are offsets relative to the start of the 'CODE' resource.
		
		offset_to_BNE       = OFFSET_TO_SPINLOOP_BNE,
		minimum_handle_size = offset_to_BNE + 2,
	};
	
	if ( handle_size > minimum_handle_size )
	{
		UInt16* p = (UInt16*) (*h + offset_to_BNE);
		
		/*
			Old:
				BNE.S    *-18    // $000738
			
			New:
				BNE.S    *-4     // $000746
		*/
		
		if ( *p == 0x66EC )
		{
			*p = 0x66FA;
			
			HNoPurge( h );
		}
	}
}

/*
	We can use fast_memcmp() / fast_memequ() here (knowing
	they're v68k-only), because fill_bytes() is similarly
	restricted, and we check before installing the patch.
	
	But call them v68k_memcmp() and v68k_memequ(),
	to avoid conflict with fast_memcmp in the enum.
*/

static inline
asm
int v68k_memcmp( void const*  a : __A0,
                 void const*  b : __A1,
                 unsigned     n : __D0 )
{
	JSR  fast_memcmp
}

static inline
int v68k_memequ( const void* a, const void* b, unsigned long n )
{
	return v68k_memcmp( a, b, n ) == 0;
}

static
asm
void blitter_sample()
{
	MOVE.W   (A2)+,D0
	MOVE.W   (A2)+,D1
	MOVE.L   (A2)+,D2
	MOVE.L   (A2)+,D3
	MOVEA.L  2(A1),A3
	ADDA.L   D2,A3
}

static
asm
void blitter_patch()
{
	MOVE.W   (A0)+,D0
	MOVE.W   (A0)+,D1
	MOVE.L   (A0)+,D2
	MOVE.L   (A0)+,D3
	MOVEA.L  2(A1),A3
	ADDA.L   D2,A3
	
	EXG      A3,A1   // load A3 to A1 (and save old A1)
	
	ADDQ.W   #1,D0   // unpredecrement row words
	ADDQ.W   #1,D1   // unpredecrement height
	ADD.W    D0,D0   // convert word count to byte count
	ADD.W    D0,D3   // convert gutter to stride
	SWAP     D0      // move src_len into place
	MOVE.W   D1,D0   // populate srcx
	MOVE.L   D0,D1   // populate dstx
	EXG      D3,D2   // populate ddst
	
	JSR      fill_bytes
	
	EXG      A3,A1   // restore A1
	EXG      A2,A0   // restore A0
}

static inline
void install_blitter_patch( Handle h, Size handle_size )
{
	enum
	{
		sample_size = 0x000e,
		patch_size  = 0x0028,
		
		// These are offsets relative to the start of the 'CODE' resource.
		
		offset_to_target    = OFFSET_TO_BLITTER_READ,
		minimum_handle_size = offset_to_target + 0x002c,
	};
	
	if ( handle_size > minimum_handle_size )
	{
		Ptr p = *h + offset_to_target;
		
		/*
			The offsets below are for Beyond Dark Castle.
			
			Old:
				0006b2:  MOVE.W   (A2)+,D0
				0006b4:  MOVE.W   (A2)+,D1
				0006b6:  MOVE.L   (A2)+,D2
				0006b8:  MOVE.L   (A2)+,D3
				0006ba:  MOVEA.L  (2,A1),A3
				0006be:  ADDA.L   D2,A3
				0006c0:  SUBQ.W         #1,D0
				0006c2:  BTST           #0,D0
				0006c6:  BEQ.S          *+22         // $0006dc
				0006c8:    ASR.W        #1,D0
				0006ca:      MOVE.W     D0,D4
				0006cc:        MOVE.L   (A2)+,(A3)+
				0006ce:      DBF        D4,*-2       // $0006cc
				0006d2:      MOVE.W     (A2)+,(A3)+
				0006d4:      ADDA.L     D3,A3
				0006d6:    DBF          D1,*-12      // $0006ca
				0006da:  BRA.S          *-54         // $0006a4
				0006dc:    ASR.W        #1,D0
				0006de:      MOVE.W     D0,D4
				0006e0:        MOVE.L   (A2)+,(A3)+
				0006e2:      DBF        D4,*-2       // $0006e0
				0006e6:      ADDA.L     D3,A3
				0006e8:    DBF          D1,*-10      // $0006de
				0006ec:  BRA.S          *-72         // $0006a4
				0006ee:
			
			Reading from A0 instead of A2 saves a word
			(since we'd otherwise have to move A2 to A0),
			allowing us to reuse the branch instruction.
			
			(A further word could be saved by swapping D2 and D3.)
			
			New:
				0006b2:  MOVE.W   (A0)+,D0
				0006b4:  MOVE.W   (A0)+,D1
				0006b6:  MOVE.L   (A0)+,D2
				0006b8:  MOVE.L   (A0)+,D3
				0006ba:  MOVEA.L  (2,A1),A3
				0006be:  ADDA.L   D2,A3
				
				0006c0:  EXG      A3,A1   // load A3 to A1 (and save old A1)
				
				0006c2:  ADDQ.W   #1,D0   // unpredecrement row words
				0006c4:  ADDQ.W   #1,D1   // unpredecrement height
				0006c6:  ADD.W    D0,D0   // convert word count to byte count
				0006c8:  ADD.W    D0,D3   // convert gutter to stride
				0006ca:  SWAP     D0      // move src_len into place
				0006cc:  MOVE.W   D1,D0   // populate srcx
				0006de:  MOVE.L   D0,D1   // populate dstx
				0006d0:  EXG      D3,D2   // populate ddst
				
				0006d2:  JSR      fill_bytes
				
				0006d6:  EXG      A3,A1   // restore A1
				0006d8:  EXG      A2,A0   // restore A0
				
				0006da:  BRA.S    *-54    // $0006a4
				0006dc:  ...
		*/
		
		if ( v68k_memequ( &blitter_sample, p, sample_size ) )
		{
			BlockMoveData( &blitter_patch, p, patch_size );
			
			HNoPurge( h );
		}
	}
}

static
asm
void fireside_sample()
{
	LINK     A6,#-12
	MOVE.L   A5,-(A7)
	MOVEA.L  0x0904,A5
	MOVE.W   #0x0108,-6(A6)
	MOVE.W   #0x01e8,-2(A6)
	MOVE.W   #0x0050,-8(A6)
	MOVE.W   #0x0111,-4(A6)
	
	CLR.W    -10(A6)
	CLR.W    -12(A6)
	
	PEA      -8(A6)       
	MOVE.L   -12(A6),-(A7)
}

static
asm
void fireside_patch()
{
	PEA      unlock_screen
	
	LINK     A6,#-12
	MOVE.L   A5,-(A7)
	MOVEA.L  0x0904,A5
	MOVE.W   #0x0108,-6(A6)
	MOVE.W   #0x01e8,-2(A6)
	MOVE.W   #0x0050,-8(A6)
	MOVE.W   #0x0111,-4(A6)
	
	JSR      lock_screen
	
	PEA      -8(A6)
	PEA      0x0000
}

static inline
void install_fireside_patch( Handle h, Size handle_size )
{
	enum
	{
		sample_size = 0x0032,
		patch_size  = 0x0032,
		
		// These are offsets relative to the start of the 'CODE' resource.
		
		offset_to_target    = 0x0f60,
		minimum_handle_size = offset_to_target + sample_size,
	};
	
	if ( handle_size > minimum_handle_size )
	{
		Ptr p = *h + offset_to_target;
		
		/*
			Old:
				000f60:  LINK     A6,#-12
				000f64:  MOVE.L   A5,-(A7)
				000f66:  MOVEA.L  0x0904,A5
				000f6a:  MOVE.W   #0x0108,(-6,A6)
				000f70:  MOVE.W   #0x01e8,(-2,A6)
				000f76:  MOVE.W   #0x0050,(-8,A6)
				000f7c:  MOVE.W   #0x0111,(-4,A6)
				
				000f82:  CLR.W    (-10,A6)
				000f86:  CLR.W    (-12,A6)
				
				000f8a:  PEA      (-8,A6)
				000f8e:  MOVE.L   (-12,A6),-(A7)
				000f92:  _ShieldCursor
				...
			
			New:
				000f60:  PEA      unlock_screen
				
				000f64:  LINK     A6,#-12
				000f68:  MOVE.L   A5,-(A7)
				000f6a:  MOVEA.L  0x0904,A5
				000f6e:  MOVE.W   #0x0108,(-6,A6)
				000f74:  MOVE.W   #0x01e8,(-2,A6)
				000f7a:  MOVE.W   #0x0050,(-8,A6)
				000f80:  MOVE.W   #0x0111,(-4,A6)
				
				000f86:  JSR      lock_screen
				
				000f8a:  PEA      (-8,A6)
				000f8e:  PEA      0x0000
				000f92:  _ShieldCursor
				...
		*/
		
		if ( v68k_memequ( &fireside_sample, p, sample_size ) )
		{
			BlockMoveData( &fireside_patch, p, patch_size );
			
			HNoPurge( h );
		}
	}
}

static inline
void install_tabpause_patch( Handle h, Size handle_size )
{
	enum
	{
		// These are offsets relative to the start of the 'CODE' resource.
		
		offset_to_trap      = OFFSET_TO_PAUSING_TRAP,
		minimum_handle_size = offset_to_trap + 6,
	};
	
	if ( handle_size > minimum_handle_size )
	{
		UInt16* p = (UInt16*) (*h + offset_to_trap);
		
		/*
			Old:
				LEA      (-130,A5),A0
				MOVE.W   #0x000a,D0
				_OSEventAvail
				TST.W    D0
				BNE.S    *-12
			
			New:
				LEA      (-130,A5),A0
				MOVE.W   #0x000a,D0
				_WaitOSEventAvail
				TST.W    D0
				BNE.S    *-12
		*/
		
		if ( *p == _OSEventAvail )
		{
			*p = _WaitOSEventAvail;
			
			HNoPurge( h );
		}
	}
}

static
void TEInit_handler()
{
	using mac::glue::GetHandleSize_raw;
	
	using mac::sys::get_trap_address;
	using mac::sys::set_trap_address;
	
	if ( Handle h = GetResource( IDENTIFYING_RESTYPE, 0 ) )
	{
		ReleaseResource( h );
		
	#ifdef SAVEGAME_CODE_RESID
		
		old_CurResFile  = get_trap_address( _CurResFile  );
		old_ExitToShell = get_trap_address( _ExitToShell );
		
		set_trap_address( (ProcPtr) CurResFile_patch,  _CurResFile  );
		set_trap_address( (ProcPtr) ExitToShell_patch, _ExitToShell );
		
	#endif
		
		if ( (h = GetResource( 'CODE', ENVCHECK_CODE_RESID )) )
		{
			install_envcheck_patch( h, GetHandleSize_raw( h ) );
		}
		
		if ( (h = GetResource( 'CODE', SPINLOOP_CODE_RESID )) )
		{
			install_spinloop_patch( h, GetHandleSize_raw( h ) );
		}
		
		const bool v68k = mac::sys::has_v68k();
		
		if ( v68k  &&  (h = GetResource( 'CODE', BLITTING_CODE_RESID )) )
		{
			install_blitter_patch( h, GetHandleSize_raw( h ) );
		}
		
	#ifdef FIRESIDE_CODE_RESID
		
		if ( v68k  &&  (h = GetResource( 'CODE', FIRESIDE_CODE_RESID )) )
		{
			install_fireside_patch( h, GetHandleSize_raw( h ) );
		}
		
	#endif
		
		if ( v68k  &&  (h = GetResource( 'CODE', 2 )) )
		{
			install_tabpause_patch( h, GetHandleSize_raw( h ) );
		}
	}
}

static
pascal asm void TEInit_patch()
{
	LINK     A6,#0
	JSR      TEInit_handler
	UNLK     A6
	
	MOVEA.L  old_TEInit,A0
	JMP      (A0)
}

int main()
{
	Handle self = GetResource( 'INIT', 0 );
	
	DetachResource( self );
	
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
