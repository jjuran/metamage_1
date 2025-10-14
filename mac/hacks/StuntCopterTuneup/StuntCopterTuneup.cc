/*
	StuntCopterTuneup.cc
	--------------------
	
	StuntCopter Tune-up INIT for Mac OS
	
	Copyright 2025, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
	This is a set of hot patches StuntCopter 1.2.
	
	On fast machines, StuntCopter 1.2 runs so quickly that
	the game is essentially unplayable.  Later versions 
	include a speed control (and a dialog box to set it) to
	address this very issue; published source code implies
	that this was added in version 1.5.
	
	The version 1.5 source code shows the speed control
	being applied in three places.  This extension patches
	just one site to achieve the same effect.  As a bonus,
	the patch eliminates a redundant EventAvail() call in
	v1.2 (which no longer exists in v1.5).
	
	(Note that the non-functional .Sound driver stub in
	at least some versions of Mac OS on PowerPC slows
	down StuntCopter 1.2 even more than this INIT does,
	so you'll need to disable sound to see the difference.)
	
	StuntCopter's About box allows the user to invoke the
	backflip animation (which also occurs in game when a
	jump is successful).  The animation occurs at 6 fps,
	with 10 ticks between frames -- which is accomplished
	by looping over TickCount() until the time has elapsed,
	thereby consuming an entire CPU core for the duration of
	the animation.  This extension patches the delay loop so
	it just calls Delay() instead, which works efficiently
	in Classic (the Blue Box) and Advanced Mac Substitute.
	
*/

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// Standard C
#include <string.h>

// mac-glue-utils
#include "mac_glue/Memory.hh"

// mac-sys-utils
#include "mac_sys/trap_address.hh"


#pragma exceptions off


static UniversalProcPtr old_InitWindows;

static const UInt16 old_backflip_timer[] =
{
	0x598f,         // 00129c:  SUBQ.L   #4,A7
	0xa975,         // 00129e:  _TickCount
	0x2c1f,         // 0012a0:  MOVE.L   (A7)+,D6
	0x0686, 0, 10,  // 0012a2:  ADDI.L   #10,D6
	0x2b46,  -164,  // 0012a8:  MOVE.L   D6,(-164,A5)
	0x598f,         // 0012ac:  SUBQ.L   #4,A7
	0xa975,         // 0012ae:  _TickCount
	0x2c1f,         // 0012b0:  MOVE.L   (A7)+,D6
	0xbcad,  -164,  // 0012b2:  CMP.L    (-164,A5),D6
	0x6f00,   -12,  // 0012b6:  BLE      *-10    // $0012ac
};

static const UInt16 new_backflip_delay[] =
{
	0x307c, 10,  // 00129c:  MOVEA.W  #10,A0
	0xa03b,      // 0012a0:  _Delay
	0x4e71,      // 0012a2:  NOP
	0x4e71,      // 0012a4:  NOP
	0x4e71,      // 0012a6:  NOP
	0x4e71,      // 0012a8:  NOP
	0x4e71,      // 0012aa:  NOP
	0x4e71,      // 0012ac:  NOP
	0x4e71,      // 0012ae:  NOP
	0x4e71,      // 0012b0:  NOP
	0x4e71,      // 0012b2:  NOP
	0x4e71,      // 0012b4:  NOP
	0x4e71,      // 0012b6:  NOP
	0x4e71,      // 0012b8:  NOP
};

static const UInt16 old_eventloop_available[] =
{
	0x558f,       // 0034be:  SUBQ.L   #2,A7
	0x3f3c,  -1,  // 0034c0:  MOVE.W   #0xffff,-(A7)
	0x486e, -16,  // 0034c4:  PEA      (-16,A6)
	0xa971,       // 0034c8:  _EventAvail
	0x1e1f,       // 0034ca:  MOVE.B   (A7)+,D7
	0x6600, 416,  // 0034cc:  BNE      *+418    // $00366e
};

static const UInt16 new_eventloop_throttled[] =
{
	0x307c, 1,  // 0034be:  MOVEA.W  #1,A0
	0xa03b,     // 0034c2:  _Delay
	0x4e71,     // 0034c4:  NOP
	0x4e71,     // 0034c6:  NOP
	0x4e71,     // 0034c8:  NOP
	0x4e71,     // 0034ca:  NOP
	0x4e71,     // 0034cc:  NOP
	0x4e71,     // 0034ce:  NOP
};

static inline
void my_memcpy( void* dst, const void* src, unsigned long n )
{
	/*
		Since we're patching a code resource that's already
		running, use BlockMove(), not BlockMoveData().
	*/
	
	BlockMove( src, dst, n );
}

static inline
bool my_memequ( const void* a, const void* b, unsigned long n )
{
	return memcmp( a, b, n ) == 0;
}

static inline
void patch( Handle h, long at, const UInt16* ante, const UInt16* post, long n )
{
	using mac::glue::GetHandleSize_raw;
	
	if ( h  &&  GetHandleSize_raw( h ) >= at + n )
	{
		Ptr p = *h + at;
		
		if ( my_memequ( p, ante, n ) )
		{
			my_memcpy( p, post, n );
		}
	}
}

#define PATCH( h, at, ante, post )  patch( h, at, ante, post, sizeof post );

static
pascal
void InitWindows_patch()
{
	if ( Handle h = GetResource( 'COPT', 0 ) )
	{
		ReleaseResource( h );
		
		h = GetResource( 'CODE', 1 );
		
		// StuntCopter 1.2
		
		PATCH( h, 0x00129c, old_backflip_timer,      new_backflip_delay      );
		PATCH( h, 0x0034be, old_eventloop_available, new_eventloop_throttled );
		
		// StuntCopter 1.5
		
		PATCH( h, 0x00131a, old_backflip_timer,      new_backflip_delay      );
	}
	
	old_InitWindows();
}

int main()
{
	enum
	{
		_InitWindows = 0xA912,
	};
	
	Handle self = GetResource( 'INIT', 0 );
	
	DetachResource( self );
	
	old_InitWindows = mac::sys::get_trap_address( _InitWindows );
	
	mac::sys::set_trap_address( (ProcPtr) InitWindows_patch, _InitWindows );
	
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
