/*
	SpectreChallengerTuneup.cc
	--------------------------
	
	Spectre Challenger Tune-up INIT for Advanced Mac Substitute
	
	Copyright 2022-2023, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
	Spectre Challenger has a spinloop that's been obfuscated by a slightly
	(but needlessly) convoluted double-branch scheme, as seen in the code
	below.  This extension patches the convoluted code into a simpler form
	that's semantically equivalent, and can be detected automatically at
	run time by xv68k and replaced with more efficient trap-based code.
	
	Additionally, Spectre Challenger's Sound Driver client code is faulty:
	It sends ffMode requests that include a 14-byte Sound Manager header
	between the 6-byte Sound Driver header and the actual audio samples,
	resulting in audible clicks when playing a sound (due to interpreting
	the Sound Manager header as audio samples).  When Spectre Challenger
	launches, this extension patches the _Write trap to intercept calls
	to the Sound Driver, temporarily rewriting the corrupted buffer and
	the I/O parameter block to remove the bogus bytes.  (Restoring them
	afterward may be unnecessary, but it's best to patch conservatively.)
	
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


short ROM85 : 0x028e;

enum
{
	kSoundDriverRefNum = -4,
};

static UniversalProcPtr  old_Write;
static UniversalProcPtr  old_TEInit;

static const UInt16 old_spinloop[] =
{
	0x102d, -29074,  // 00372a:  MOVE.B   (-29074,A5),D0
	0x6702,          // 00372e:  BEQ.S    *+4    // $003732
	0x60f8,          // 003730:  BRA.S    *-6    // $00372a
};

static const UInt16 new_spinloop[] =
{
	0x4a2d, -29074,  // 00372a:  TST.B    (-29074,A5)
	0x66fa,          // 00372e:  BNE.S    *-4    // $00372a
	0x4e71,          // 003730:  NOP
};

static const UInt16 corrupt_header[] =
{
	0x0000,          // ffMode
	0x0001, 0x0000,  // rate
	0x56ee, 0x8ba3,  // 22254.[54] as a Fixed
	0x0000,
};

static inline
bool corrupted( const void* buffer )
{
	return fast_memcmp( buffer, corrupt_header, sizeof corrupt_header ) == 0;
}

static inline
asm
short call_IO_handler( void* proc : __A1, IOParam* pb : __A0 )
{
	JSR      (A1)
}

static
short Write_to_Sound_Driver( IOParam* pb : __A0 )
{
	Ptr buffer = pb->ioBuffer;
	UInt32 req = pb->ioReqCount;
	
	UInt16 saved_words[ 3 ];
	
	Ptr new_start = buffer + 14;
	
	fast_memcpy( saved_words, new_start, 6 );
	
	if ( corrupted( buffer ) )
	{
		fast_memcpy( new_start, buffer, 6 );
		
		pb->ioBuffer = new_start;
		pb->ioReqCount -= 14;
	}
	
	OSErr err = call_IO_handler( old_Write, pb );
	
	fast_memcpy( new_start, saved_words, 6 );
	
	pb->ioBuffer   = buffer;
	pb->ioReqCount = req;
	
	return err;
}

static
asm
short Write_patch( IOParam* pb : __A0 )
{
	CMPI.W   #kSoundDriverRefNum,24(A0)
	BNE.S    not_Sound
	
	JMP      Write_to_Sound_Driver
	
not_Sound:
	
	// call previous Write handler
	MOVE.L   old_Write,-(SP)
	
	RTS
}

static inline
void install_patches( Handle h )
{
	old_Write = mac::sys::get_trap_address( _Write );
	
	mac::sys::set_trap_address( (ProcPtr) Write_patch, _Write );
	
	const int query_offset = 0x00372a;
	
	if ( h  &&  GetHandleSize( h ) >= query_offset + sizeof old_spinloop )
	{
		UInt32* p = (UInt32*) (*h + query_offset);
		
		if ( fast_memequ( p, old_spinloop, sizeof old_spinloop ) )
		{
			fast_memcpy( p, new_spinloop, sizeof new_spinloop );
			
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
	if ( ROM85 > 0 )
	{
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
