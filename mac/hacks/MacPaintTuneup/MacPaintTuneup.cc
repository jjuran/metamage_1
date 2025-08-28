/*
	MacPaintTuneup.cc
	-----------------
	
	MacPaint Tune-up INIT for Advanced Mac Substitute
	
	Copyright 2025, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
	This extension is a set of hot patches for MacPaint that
	improve performance when running in Advanced Mac Substitute.
	
	When using the hand tool to scroll the document, rather
	than call ScrollRect() (which calls CopyBits()), MacPaint
	uses its own blitter that draws directly to the screen,
	one longword at a time, resulting in noticeable tearing.
	
	Since the blitting operation is bracketed by calls to
	ShieldCursor() and ShowCursor(), we can patch them to
	lock the screen for updates in between.  ShowCursor()
	is also sometimes paired with HideCursor(), so we also
	have to patch the latter for parity with the former.
	
*/

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif
#ifndef __TRAPS__
#include <Traps.h>
#endif

// mac-sys-utils
#include "mac_sys/trap_address.hh"

// ams-common
#include "callouts.hh"


#pragma exceptions off


typedef pascal void (*ShieldCursor_ProcPtr)( const Rect* rect, Point offset );

static UniversalProcPtr  old_ExitToShell;
static UniversalProcPtr  old_HideCursor;
static UniversalProcPtr  old_ShieldCursor;
static UniversalProcPtr  old_ShowCursor;
static UniversalProcPtr  old_TEInit;

static
pascal
void ShieldCursor_patch( const Rect* rect, Point offset )
{
	lock_screen();
	
	ShieldCursor_ProcPtr ShieldCursor = (ShieldCursor_ProcPtr) old_ShieldCursor;
	
	ShieldCursor( rect, offset );
}

static
void HideCursor_patch()
{
	lock_screen();
	
	old_HideCursor();
}

static
void ShowCursor_patch()
{
	old_ShowCursor();
	
	unlock_screen();
}

static
void ExitToShell_patch()
{
	mac::sys::set_trap_address( old_ExitToShell,  _ExitToShell  );
	mac::sys::set_trap_address( old_HideCursor,   _HideCursor   );
	mac::sys::set_trap_address( old_ShieldCursor, _ShieldCursor );
	mac::sys::set_trap_address( old_ShowCursor,   _ShowCursor   );
	
	asm
	{
		DC.W     _ExitToShell | 0x400  // autoPop
	}
}

static inline
void install_trap_patches()
{
	old_ExitToShell  = mac::sys::get_trap_address( _ExitToShell  );
	old_HideCursor   = mac::sys::get_trap_address( _HideCursor   );
	old_ShieldCursor = mac::sys::get_trap_address( _ShieldCursor );
	old_ShowCursor   = mac::sys::get_trap_address( _ShowCursor   );
	
	mac::sys::set_trap_address( (ProcPtr) ExitToShell_patch,  _ExitToShell  );
	mac::sys::set_trap_address( (ProcPtr) HideCursor_patch,   _HideCursor   );
	mac::sys::set_trap_address( (ProcPtr) ShieldCursor_patch, _ShieldCursor );
	mac::sys::set_trap_address( (ProcPtr) ShowCursor_patch,   _ShowCursor   );
}

static
pascal
void TEInit_patch()
{
	if ( Handle h = Get1Resource( 'MPNT', 0 ) )
	{
		ReleaseResource( h );
		
		install_trap_patches();
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
