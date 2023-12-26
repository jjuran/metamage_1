/*
	ProCoTuneup.cc
	--------------
	
	Professional Composer Tune-up INIT for Mac OS
	
	Copyright 2023, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
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


static UniversalProcPtr old_TEInit;

static const UInt16 ProCo_wait_loop[] =
{
	0x42A7,          // 002670:  CLR.L    -(A7)
	0xA975,          // 002672:  _TickCount
	0x302D, -9856,   // 002674:  MOVE.W   (-9856,A5),D0
	0x48C0,          // 002678:  EXT.L    D0
	0xD0AD, -9854,   // 00267a:  ADD.L    (-9854,A5),D0
	0xB09F,          // 00267e:  CMP.L    (A7)+,D0
	0x6D02,          // 002680:  BLT.S    *+4     ; $002684
	0x60EC,          // 002682:  BRA.S    *-18    ; $002670
};

static const UInt16 ProCo_wait_trap[] =
{
	0x306D, -9856,   // 002670:  MOVEA.W  (-9856,A5),A0
	0xD1ED, -9854,   // 002674:  ADDA.L   (-9854,A5),A0
	0x91F8, 0x016A,  // 002678:  SUBA.L   Ticks,A0
	0x2008,          // 00267c:  MOVE.L   A0,D0
	0x6D04,          // 00267e:  BLT.S    *+6     ; $002684
	0x5288,          // 002688:  ADDQ.L   #1,A0
	0xA03B,          // 002682:  _Delay
};

static inline
void my_memcpy( void* dst, const void* src, long n )
{
	BlockMoveData( src, dst, n );
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
	const int query_offset = 0x002670;
	
	if ( h  &&  GetHandleSize( h ) >= query_offset + sizeof ProCo_wait_loop )
	{
		Ptr p = *h + query_offset;
		
		if ( equal_words( (UInt16*) p, VEC_LEN( ProCo_wait_loop ) ) )
		{
			my_memcpy( p, ProCo_wait_trap, sizeof ProCo_wait_trap );
			
			HNoPurge( h );
		}
	}
}

static
pascal
void TEInit_patch()
{
	if ( Handle h = GetResource( 'MOTU', 0 ) )
	{
		ReleaseResource( h );
		
		install_patch( GetResource( 'CODE', 5 ) );
	}
	
	old_TEInit();
}

int main()
{
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
