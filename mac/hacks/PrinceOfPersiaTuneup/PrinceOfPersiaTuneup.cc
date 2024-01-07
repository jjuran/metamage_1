/*
	PrinceOfPersiaTuneup.cc
	-----------------------
	
	Prince of Persia Tune-up INIT for Mac OS
	
	Copyright 2022-2023, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
	Prince of Persia for Mac OS has an off-by-one error in the timing
	loop that determines how long a frame lasts, which increases the
	duration of each frame by one tick and makes gameplay sluggish.
	
	(See <https://forum.princed.org/viewtopic.php?f=63&t=3009&start=15>.)
	
	Additionally, the same timing loop does nothing but call TickCount()
	until some number of ticks has elapsed, but it does it by constantly
	executing instructions.  In a virtual environment where idling saves
	power or frees up computing resources for other processes (or both),
	it's advantageous to do so in preference to busy-looping.  Examples
	of such environments known to benefit include Mac OS X's Classic
	(i.e. the Blue Box) and the author's own Advanced Mac Substitute.
	
	This is a hot patch for Prince of Persia that fixes the off-by-one
	error by replacing the faulty BLE instruction with a BLT instruction.
	It also modifies the timing loop to call Delay() each time through,
	reducing CPU use (and consequently, power consumption) considerably.
	
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


#define LENGTH(array)  (sizeof (array) / sizeof (array)[0])

#define VEC_LEN(array)  array, LENGTH(array)


short ROM85 : 0x028e;

static UniversalProcPtr old_TEInit;

static const UInt16 PoP_wait_loop[] =
{
	0x42A7,          // 0000f2:  CLR.L    -(A7)
	0xA975,          // 0000f4:  _TickCount
	0x2B5F, -23198,  // 0000f6:  MOVE.L   (A7)+,(-23198,A5)
	0x4EBA,  11026,  // 0000fa:  JSR      *+11028    ; $002c0e
	0x202D, -23198,  // 0000fe:  MOVE.L   (-23198,A5),D0
	0x90AD, -23202,  // 000102:  SUB.L    (-23202,A5),D0
	0xB0AD, -27060,  // 000106:  CMP.L    (-27060,A5),D0
	0x6FE6,          // 00010a:  BLE.S    *-24    ; $0000f2
};

static
asm
void idle()
{
	// Reproduce clobbered instructions' effect
	MOVE.L   0x016A,-23198(A5)
	
	// Spend a litle time quiescent rather than busy looping
	MOVEA.L  #1,A0
	_Delay
	
	RTS
}

static inline
bool equal_words( const UInt16* a, const UInt16* b, short n )
{
	do
	{
		if ( *a++ != *b++ )
		{
			return false;
		}
	}
	while ( --n > 0 );
	
	return true;
}

static inline
void install_patch( Handle h )
{
	const int query_offset = 0x0000f2;
	
	if ( h  &&  GetHandleSize( h ) >= query_offset + sizeof PoP_wait_loop )
	{
		Ptr p = *h + query_offset;
		
		if ( equal_words( (UInt16*) p, VEC_LEN( PoP_wait_loop ) ) )
		{
			/*
				Graft a call to our idle routine into the busy loop to save
				power and CPU in case we're running in a virtual environment.
			*/
			
			UInt32* q = (UInt32*) p;
			
			*q++ = 0x4E714EB9;  // NOP; JSR
			*q++ = (UInt32) &idle;
			
			p += sizeof PoP_wait_loop - 2;
			
			*p = 0x6D;  // BLT, not BLE
			
			HNoPurge( h );
		}
	}
}

static
pascal
void TEInit_patch()
{
	if ( Handle h = Get1Resource( 0x506fc450, 0 ) )
	{
		ReleaseResource( h );
		
		install_patch( Get1Resource( 'CODE', 2 ) );
	}
	
	old_TEInit();
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
