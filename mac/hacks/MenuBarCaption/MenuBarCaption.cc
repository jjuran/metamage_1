/*
	MenuBarCaption.cc
	-----------------
	
	Menu Bar Caption INIT for classic Mac OS
	
	Copyright 2026, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
*/

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif
#ifndef __QUICKDRAWTEXT__
#include <QuickdrawText.h>
#endif
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// mac-sys-utils
#include "mac_sys/trap_address.hh"


enum
{
	_DrawMenuBar = 0xA937,
	_ExitToShell = 0xA9F4,
	_TEInit      = 0xA9CC,
};

Rect    CrsrPin  : 0x0834;
GrafPtr WMgrPort : 0x09DE;
Handle  MenuList : 0x0A1C;

static UniversalProcPtr old_DrawMenuBar;
static UniversalProcPtr old_ExitToShell;
static UniversalProcPtr old_TEInit;


enum
{
	kVersionStringResourceID = 'v' << 8 | '#',  // $7623, a.k.a. 9078
};

static
void DrawMenuBar_patch()
{
	asm
	{
		MOVEA.L  old_DrawMenuBar,A0
		JSR      (A0)
	}
	
	if ( *(short*) *MenuList )
	{
		return;  // don't draw if menus exist
	}
	
	if ( Handle r = GetResource( 'STR ', kVersionStringResourceID ) )
	{
		GrafPtr saved_port;
		
		GetPort( &saved_port );
		
		SetPort( WMgrPort );
		
		HLock( r );
		
		const Byte* caption = (const Byte*) *r;
		
		short width = StringWidth( caption );
		
		short h = (CrsrPin.right - width) / 2u;
		short v = 14;
		
		MoveTo( h, v );
		
		DrawString( caption );
		
		HUnlock( r );
		
		SetPort( saved_port );
	}
}

static
void ExitToShell_patch()
{
	using mac::sys::set_trap_address;
	
	set_trap_address( old_DrawMenuBar, _DrawMenuBar );
	set_trap_address( old_ExitToShell, _ExitToShell );
	
	asm
	{
		DC.W     _ExitToShell | 0x400  // autoPop
	}
}

static
void TEInit_handler()
{
	using mac::sys::get_trap_address;
	using mac::sys::set_trap_address;
	
	old_DrawMenuBar  = get_trap_address( _DrawMenuBar  );
	old_ExitToShell  = get_trap_address( _ExitToShell  );
	
	set_trap_address( (ProcPtr) DrawMenuBar_patch, _DrawMenuBar );
	set_trap_address( (ProcPtr) ExitToShell_patch, _ExitToShell );
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
	using mac::sys::get_trap_address;
	using mac::sys::set_trap_address;
	
	Handle self = GetResource( 'INIT', 0 );
	
	DetachResource( self );
	
	old_TEInit = get_trap_address( _TEInit );
	
	set_trap_address( (ProcPtr) TEInit_patch, _TEInit );
	
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
