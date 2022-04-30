/*
	LRCrashFix.cc
	-------------
	
	Lode Runner Crash Fix INIT for Advanced Mac Substitute
	
	Copyright 2022, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
	This is an application hot patch to prevent a crash in Lode Runner
	due to an unmapped memory access when run in Advanced Mac Substitute.
	
	The problem is that Lode Runner's call to FindControl() is followed by
	an instruction that mishandles the result:  Instead of storing it in a
	dummy variable (which is presumably what was intended), it writes the
	word into unmapped memory.
	
	On actual Macintosh hardware, this causes no problem because (a) the
	result is never looked at (in either location) so its absence isn't
	missed, and (b) memory accesses never fail on 68000-based Macs.  Since
	$ffae28 is in unmapped I/O space, writes to it are simply ignored.
	
	A subsequent TrackControl() call is also followed by an instruction that
	stores its result (after popping it off the stack), but this one actually
	does write it to the very same dummy variable that the FindControl() call
	missed.
	
	(I'm not sure how this happened; it's almost certainly not a compiler
	bug, seeing how the correct instruction also occurs later in the same
	function, but it seems a rather odd error to make when writing assembly
	language by hand.  Perhaps the binary bitrotted before duplication?)
	
	003afe:  _FindControl
	003b00:  MOVE.W   (SP)+,0xae28       // $31df ae28
	003b04:  BEQ      *+26               // if (Z) goto $003b1e
	...
	003b14:  _TrackControl
	003b16:  MOVE.W   (SP)+,(-20952,A5)  // $3b5f ae28
	003b1a:  BNE      *+8                // if (!Z) goto $003b22
	
	Note that $ae28 and -20952 are the same bit pattern, just disassembled
	differently.  Both instructions correctly pop the result off of the
	stack, but one writes it to an application global (via an A5-relative
	reference) and the other to the absolute address $ffffae28.  Both times,
	the result is checked by using conditional branches, so to that end it
	doesn't matter where the result goes as long as the CCR gets updated.
	
	However, xv68k is rather strict about what address ranges are defined
	for memory access, so clicking Lode Runner's game window writes to
	unmapped memory and halts the emulator.  To avoid that, we'll hot-patch
	the broken instruction.
	
	This code is designed to run only as an 'INIT' resource in the System
	file (or equivalent, viz. AMS Resources).  It's not needed in Mac OS,
	and will definitely crash Mac OS if installed there as an 'INIT' file.
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


#pragma exceptions off


enum
{
	long_pop_to_absolute_short = 0x31DF,  // dst: mode 7, reg 0
	long_pop_to_displaced_A5   = 0x3B5F,  // dst: mode 5, reg 5
};

static UniversalProcPtr old_TEInit;

static
void TEInit_handler()
{
	if ( Handle h = Get1Resource( 'GLEN', 0 ) )
	{
		ReleaseResource( h );
		
		const int query_offset = 0x003afe;
		
		h = Get1Resource( 'CODE', 1 );
		
		const Size size = GetHandleSize( h );
		
		if ( size < query_offset + sizeof (UInt16) * 2 )
		{
			return;
		}
		
		UInt16* p = (UInt16*) (*h + query_offset);
		
		if ( *p++ == _FindControl  &&  *p == long_pop_to_absolute_short )
		{
			*p = long_pop_to_displaced_A5;
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
