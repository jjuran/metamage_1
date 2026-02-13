/*
	CastleTuneup.cc.hh
	------------------
	
	Dark Castle Tune-up INIT template for Advanced Mac Substitute
	
	Copyright 2020-2026, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
	This is a set of application hot patches for Dark Castle
	and Beyond Dark Castle.  The two games share a great deal
	of code (with similar sets of compatibility issues with
	Advanced Mac Substitute), but in different locations.
	
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


static UniversalProcPtr old_TEInit;


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
	
	if ( Handle h = GetResource( IDENTIFYING_RESTYPE, 0 ) )
	{
		ReleaseResource( h );
		
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
	enum
	{
		_TEInit = 0xA9CC,
	};
	
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
