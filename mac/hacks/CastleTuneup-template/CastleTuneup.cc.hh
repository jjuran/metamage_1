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
	This patch NOPs out the JSR instruction, preventing the access.
	
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
	
*/

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// mac-glue-utils
#include "mac_glue/Memory.hh"

// mac-sys-utils
#include "mac_sys/trap_address.hh"


enum
{
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
	enum
	{
		// These are offsets relative to the start of the 'CODE' resource.
		
		offset_to_JSR       = OFFSET_TO_ENVCHECK_JSR,
		minimum_handle_size = offset_to_JSR + 4,
	};
	
	if ( handle_size > minimum_handle_size )
	{
		UInt32* p = (UInt32*) (*h + offset_to_JSR);
		
		/*
			Old:
				JSR      (114,A5)
			
			New:
				NOP
				NOP
		*/
		
		if ( *p == ENVCHECK_JSR )
		{
			*p = 0x4E714E71;
			
			HNoPurge( h );
		}
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
