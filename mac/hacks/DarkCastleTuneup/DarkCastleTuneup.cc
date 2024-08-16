/*
	DarkCastleTuneup.cc
	-------------------
	
	Dark Castle Tune-up INIT for Mac OS
	
	Copyright 2020-2024, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
	This is an application hot patch for Dark Castle.  Note that
	the Dark Castle application is a shim that launches "Data A",
	which is the real application.  Also note that both have the
	'DCAS' owner resource, although only Data A has 'CODE' id=3.
	
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
	
*/

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// mac-glue-utils
#include "mac_glue/Memory.hh"

// mac-sys-utils
#include "mac_sys/trap_address.hh"


#endif

static UniversalProcPtr old_TEInit;


static inline
void install_envcheck_patch( Handle h, Size handle_size )
{
	enum
	{
		// These are offsets relative to the start of the 'CODE' resource.
		
		offset_to_JSR       = 0x0496,
		minimum_handle_size = offset_to_JSR + 4,
	};
	
	if ( h  &&  handle_size > minimum_handle_size )
	{
		UInt32* p = (UInt32*) (*h + offset_to_JSR);
		
		/*
			Old:
				JSR      (114,A5)
			
			New:
				NOP
				NOP
		*/
		
		if ( *p == 0x4ead0072 )
		{
			*p = 0x4E714E71;
			
			HNoPurge( h );
		}
	}
}

static
void TEInit_handler()
{
	if ( Handle h = GetResource( 'DCAS', 0 ) )
	{
		ReleaseResource( h );
		
		if ( (h = GetResource( 'CODE', 3 )) )
		{
			Size size = mac::glue::GetHandleSize_raw( h );
			
			install_envcheck_patch( h, size );
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
