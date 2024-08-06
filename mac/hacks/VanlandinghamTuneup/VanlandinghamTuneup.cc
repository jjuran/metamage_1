/*
	VanlandinghamTuneup.cc
	----------------------
	
	Vanlandingham Tune-up INIT for Mac OS
	
	Copyright 2024, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
	This is an application hot patch for Vanlandingham.
	It replaces a 20-word busy loop with a 4-word Delay call.
	
	Although Vanlandingham's "sleep" function is only called
	once at startup (for 10 ticks) and once on Quit (for 12 ticks),
	the patch does measurably reduce CPU time by those amounts
	when run in Advanced Mac Substitute.
	
*/

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// mac-glue-utils
#include "mac_glue/Memory.hh"

// mac-sys-utils
#include "mac_sys/has/virtualization.hh"
#include "mac_sys/trap_address.hh"


#pragma exceptions off


#define LENGTH(array)  (sizeof (array) / sizeof (array)[0])

#define VEC_LEN(array)  array, LENGTH(array)


static UniversalProcPtr old_InitFonts;

static const UInt16 shipped_sleep_loop[] =
{
	0x4e56, -4,  // 0000dc:  LINK     A6,#-4
	0x2f07,      // 0000e0:  MOVE.L   D7,-(A7)
	0x42a7,      // 0000e2:  CLR.L    -(A7)
	0xa975,      // 0000e4:  _TickCount
	0x2e1f,      // 0000e6:  MOVE.L   (A7)+,D7
	0x302e,  8,  // 0000e8:  MOVE.W   (8,A6),D0
	0x48c0,      // 0000ec:  EXT.L    D0
	0xde80,      // 0000ee:  ADD.L    D0,D7
	0x42a7,      // 0000f0:  CLR.L    -(A7)
	0xA975,      // 0000f2:  _TickCount
	0xbe9f,      // 0000f4:  CMP.L    (A7)+,D7
	0x6f02,      // 0000f6:  BLE.S    *+4       // $0000fa
	0x60f6,      // 0000f8:  BRA.S    *-8       // $0000f0
	0x2e1f,      // 0000fa:  MOVE.L   (A7)+,D7
	0x4e5e,      // 0000fc:  UNLK     A6
	0x205f,      // 0000fe:  MOVEA.L  (A7)+,A0
	0x544f,      // 000100:  ADDQ.W   #2,A7
	0x4ed0,      // 000102:  JMP      (A0)
};

static const UInt16 patched_sleep_trap[] =
{
	0x225f,  // 0000dc:  MOVEA.L  (A7)+,A1
	0x305f,  // 0000de:  MOVEA.W  (A7)+,A0
	0xa03b,  // 0000e0:  _Delay
	0x4ed1,  // 0000e2:  JMP      (A1)
	0x4e71,  // 0000e4:  NOP
	0x4e71,  // 0000e6:  NOP
	0x4e71,  // 0000e8:  NOP
	0x4e71,  // 0000ea:  NOP
	0x4e71,  // 0000ec:  NOP
	0x4e71,  // 0000ee:  NOP
	0x4e71,  // 0000f0:  NOP
	0x4e71,  // 0000f2:  NOP
	0x4e71,  // 0000f4:  NOP
	0x4e71,  // 0000f6:  NOP
	0x4e71,  // 0000f8:  NOP
	0x4e71,  // 0000fa:  NOP
	0x4e71,  // 0000fc:  NOP
	0x4e71,  // 0000fe:  NOP
	0x4e71,  // 000100:  NOP
	0x4e71,  // 000102:  NOP
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
void install_sleep_patch( Handle h, Size handle_size )
{
	if ( handle_size >= 0x000104 )
	{
		UInt16* p = (UInt16*) (*h + 0x0000dc);
		
		if ( equal_words( p, VEC_LEN( shipped_sleep_loop ) ) )
		{
			my_memcpy( p, patched_sleep_trap, sizeof patched_sleep_trap );
		}
	}
}

static
void InitFonts_handler()
{
	if ( Handle h = Get1Resource( 'VANL', 0 ) )
	{
		ReleaseResource( h );
		
		if ( (h = Get1Resource( 'CODE', 1 )) )
		{
			Size size = mac::glue::GetHandleSize_raw( h );
			
			install_sleep_patch( h, size );
		}
	}
}

static
pascal asm void InitFonts_patch()
{
	LINK     A6,#0
	JSR      InitFonts_handler
	UNLK     A6
	
	MOVEA.L  old_InitFonts,A0
	JMP      (A0)
}

int main()
{
	enum
	{
		_InitFonts = 0xA8FE,
	};
	
	if ( mac::sys::has_v68k() )
	{
		Handle self = Get1Resource( 'INIT', 0 );
		
		DetachResource( self );
		
		old_InitFonts = mac::sys::get_trap_address( _InitFonts );
		
		mac::sys::set_trap_address( (ProcPtr) InitFonts_patch, _InitFonts );
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
