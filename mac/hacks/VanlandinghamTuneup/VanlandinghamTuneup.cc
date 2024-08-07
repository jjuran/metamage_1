/*
	VanlandinghamTuneup.cc
	----------------------
	
	Vanlandingham Tune-up INIT for Mac OS
	
	Copyright 2024, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
	This is a pair of application hot patches for Vanlandingham.
	One replaces a 20-word busy loop with a 4-word Delay call.
	Another rewrites a dissolve routine to use only non-volatile
	registers across a Toolbox trap call to HideCursor().
	
	Although Vanlandingham's "sleep" function is only called
	once at startup (for 10 ticks) and once on Quit (for 12 ticks),
	the patch does measurably reduce CPU time by those amounts
	when run in Advanced Mac Substitute.
	
	HideCursor() is a Toolbox trap call, which is allowed to clobber
	the usual volatile registers: D0-D2 and A0-A1.  In practice,
	the implementation in Advanced Mac Substitute would clobber A0
	had it not been written in assembly language to avoid this very
	outcome.  This patch allows the asm rewrite to be reverted.
	
	(Earlier in the development of Advanced Mac Substitute, the
	HideCursor() implementation did clobber other registers, which
	caused Vanlandingham to crash, necessitating the asm rewrite.
	Now, the only symptom would be drawing garbage to the screen
	on account of the clobbered source address.  Presumably, Mac OS'
	HideCursor() implementation in early ROMs doesn't clobber any of
	the volatile registers (except possibly D0, which Vanlandingham
	doesn't use here), but nothing prevents an INIT from patching it
	with its own code that does.
	
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

static const UInt16 shipped_byte_dissolve_A[] =
{
	0x4e56,      0,  // 002566:  LINK     A6,#0
	0x48e7, 0x1020,  // 00256a:  MOVEM.L  D3/A2,-(A7)
};

static const UInt16 shipped_byte_dissolve_B[] =
{
	0x45fa, 0x002a,          // 0025c4:  LEA      *+44,A2    ; $0025f0
	0x2232, 0x0000,          // 0025c8:  MOVE.L   (A2,D0.W),D1
	0x2401,                  // 0025cc:  MOVE.L   D1,D2
	0x2050,                  // 0025ce:  MOVEA.L  (A0),A0     // srcBase
	0x2251,                  // 0025d0:  MOVEA.L  (A1),A1     // dstBase
	0xa852,                  // 0025d2:  _HideCursor
	0x6002,                  // 0025d4:  BRA.S    *+4    ; $0025d8
	0xb342,                  // 0025d6:  EOR.W    D1,D2
	0xb642,                  // 0025d8:  CMP.W    D2,D3
	0x6f06,                  // 0025da:  BLE.S    *+8    ; $0025e2
	0x13b0, 0x2000, 0x2000,  // 0025dc:  MOVE.B   (A0,D2.W),(A1,D2.W)
	0xe24a,                  // 0025e2:  LSR.W    #1,D2
	0x62f6,                  // 0025e4:  BHI.S    *-8    ; $0025dc
	0x66ee,                  // 0025e6:  BNE.S    *-16    ; $0025d6
	0x1290,                  // 0025e8:  MOVE.B   (A0),(A1)
	0xa853,                  // 0025ea:  _ShowCursor
	0x4cdf, 0x0408,          // 0025ec:  MOVEM.L  (A7)+,D3/A2
	0x4e5e,                  // 0025f0:  UNLK     A6
};

static const UInt16 patched_byte_dissolve_A[] =
{
	0x4e56,      0,  // 002566:  LINK     A6,#0
	0x48e7, 0x1c30,  // 00256a:  MOVEM.L  D3-D5/A2-A3,-(A7)
};

static const UInt16 patched_byte_dissolve_B[] =
{
	0x45fa, 0x002a,          // 0025c4:  LEA      *+44,A2    ; $0025f0
	0x2832, 0x0000,          // 0025c8:  MOVE.L   (A2,D0.W),D4
	0x2a04,                  // 0025cc:  MOVE.L   D4,D5
	0x2450,                  // 0025ce:  MOVEA.L  (A0),A2     // srcBase
	0x2651,                  // 0025d0:  MOVEA.L  (A1),A3     // dstBase
	0xa852,                  // 0025d2:  _HideCursor
	0x6002,                  // 0025d4:  BRA.S    *+4    ; $0025d8
	0xb945,                  // 0025d6:  EOR.W    D4,D5
	0xb645,                  // 0025d8:  CMP.W    D5,D3
	0x6f06,                  // 0025da:  BLE.S    *+8    ; $0025e2
	0x17b2, 0x5000, 0x5000,  // 0025dc:  MOVE.B   (A2,D5.W),(A3,D5.W)
	0xe24d,                  // 0025e2:  LSR.W    #1,D5
	0x62f6,                  // 0025e4:  BHI.S    *-8    ; $0025dc
	0x66ee,                  // 0025e6:  BNE.S    *-16    ; $0025d6
	0x1692,                  // 0025e8:  MOVE.B   (A2),(A3)
	0xa853,                  // 0025ea:  _ShowCursor
	0x4cdf, 0x0c38,          // 0025ec:  MOVEM.L  (A7)+,D3-D5/A2-A3
	0x4e5e,                  // 0025f0:  UNLK     A6
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

static inline
void install_byte_dissolve_patch( Handle h, Size handle_size )
{
	if ( handle_size >= 0x0025f2 )
	{
		UInt16* pA = (UInt16*) (*h + 0x002566);
		UInt16* pB = (UInt16*) (*h + 0x0025c4);
		
		bool patch_byte_dissolve =
			equal_words( pA, VEC_LEN( shipped_byte_dissolve_A ) )  &&
			equal_words( pB, VEC_LEN( shipped_byte_dissolve_B ) );
		
		if ( patch_byte_dissolve )
		{
			my_memcpy( pA, patched_byte_dissolve_A, sizeof patched_byte_dissolve_A );
			my_memcpy( pB, patched_byte_dissolve_B, sizeof patched_byte_dissolve_B );
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
			
			install_byte_dissolve_patch( h, size );
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
