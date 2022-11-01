/*
	SpectreTuneup.cc
	----------------
	
	Spectre Challenger Tune-up INIT for Advanced Mac Substitute
	
	Copyright 2022, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
	Spectre Challenger has a spinloop that's been obfuscated by a slightly
	(but needlessly) convoluted double-branch scheme, as seen in the code
	below.  This extension patches the convoluted code into a simpler form
	that's semantically equivalent, and can be detected automatically at
	run time by xv68k and replaced with more efficient trap-based code.
	
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


static UniversalProcPtr  old_TEInit;

static const UInt16 old_wait_loop[] =
{
	0x102d, -29074,  // 00372a:  MOVE.B   (-29074,A5),D0
	0x6702,          // 00372e:  BEQ.S    *+4    // $003732
	0x60f8,          // 003730:  BRA.S    *-6    // $00372a
};

static const UInt16 new_wait_loop[] =
{
	0x4a2d, -29074,  // 00372a:  TST.B    (-29074,A5)
	0x66fa,          // 00372e:  BNE.S    *-4    // $003732
	0x4e71,          // 003730:  NOP
};

static inline
void install_patches( Handle h )
{
	const int query_offset = 0x00372a;
	
	if ( h  &&  GetHandleSize( h ) >= query_offset + sizeof old_wait_loop )
	{
		UInt32* p = (UInt32*) (*h + query_offset);
		
		if ( fast_memequ( p, old_wait_loop, sizeof old_wait_loop ) )
		{
			fast_memcpy( p, new_wait_loop, sizeof new_wait_loop );
			
			HNoPurge( h );
		}
	}
}

static
pascal
void TEInit_patch()
{
	if ( Handle h = Get1Resource( 'SpeC', 0 ) )
	{
		ReleaseResource( h );
		
		install_patches( Get1Resource( 'CODE', 10 ) );
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
