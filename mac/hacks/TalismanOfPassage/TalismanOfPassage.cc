/*
	TalismanOfPassage.cc
	--------------------
	
	Talisman of Passage INIT for Mac OS
	
	Copyright 2024, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
		When death's the prize for choosing poorly
		Faced with many brews,
		The wisest course of action
		Is to never have to choose.
	
*/

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// mac-glue-utils
#include "mac_glue/Memory.hh"

// mac-sys-utils
#include "mac_sys/trap_address.hh"


#pragma exceptions off


short ROM85 : 0x028e;

static UniversalProcPtr old_TEInit;

static inline
void install_patch( Handle h )
{
	using mac::glue::GetHandleSize_raw;
	
	const int query_offset_1 = 0x001f72;
	const int query_offset_2 = 0x0020a6;
	
	if ( h  &&  GetHandleSize_raw( h ) >= query_offset_2 + sizeof (UInt16) )
	{
		UInt16* p;
		
		p = (UInt16*) (*h + query_offset_1);
		
		if ( *p == 0x3B7C )
		{
			/*
				Old:  MOVE.W   #0x000f,(-17820,A5)
				New:  BRA.S    *+6     // $001f78
			*/
			
			*p = 0x6004;
			
			HNoPurge( h );
		}
		
		p = (UInt16*) (*h + query_offset_2);
		
		if ( *p == 0x6606 )
		{
			/*
				Old:  BNE.S    *+8     // $0020ae
				New:  BRA.S    *+8     // $0020ae
			*/
			
			*p = 0x6006;
			
			HNoPurge( h );
		}
	}
}

static
void TEInit_handler()
{
	if ( Handle h = Get1Resource( 0x506fc450, 0 ) )
	{
		ReleaseResource( h );
		
		install_patch( Get1Resource( 'CODE', 2 ) );
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
	
	if ( ROM85 > 0 )
	{
		Handle self = Get1Resource( 'INIT', 0 );
		
		DetachResource( self );
		
		old_TEInit = mac::sys::get_trap_address( _TEInit );
		
		mac::sys::set_trap_address( (ProcPtr) TEInit_patch, _TEInit );
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
