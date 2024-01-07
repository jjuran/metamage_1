/*
	PotionOfTrueSeeing.cc
	---------------------
	
	Potion of True Seeing INIT for Mac OS
	
	Copyright 2022, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
		When runes of death confuse you
		And it seems there's no way out
		This brew will clear your vision
		So you won't have any doubt.
	
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


short ROM85 : 0x028e;

static UniversalProcPtr old_TEInit;

static inline
void install_patch( Handle h )
{
	const int query_offset = 0x001b7e;
	
	if ( h  &&  GetHandleSize( h ) >= query_offset + sizeof (UInt32) )
	{
		UInt32* p = (UInt32*) (*h + query_offset);
		
		if ( *p == 0x7c006018 )
		{
			/*
				Old:  MOVEQ    #0,D6
				      BRA.S    *+26    // $001b9a
				New:  MOVEQ    #32,D0
				      BRA.S    *+38    // $001ba6
			*/
			
			*p = 0x70206024;
			
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
		
		install_patch( Get1Resource( 'CODE', 3 ) );
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
