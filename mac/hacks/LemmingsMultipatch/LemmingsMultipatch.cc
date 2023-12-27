/*
	LemmingsMultipatch.cc
	---------------------
	
	Lemmings Multipatch INIT for Mac OS
	
	Copyright 2022-2023, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
	This is a pair of application hot patches for Lemmings.  One of them
	prevents a crash after sound-enabled gameplay ends due to a dereference
	of the NULL address in Advanced Mac Substitute (which doesn't tolerate
	such nonsense).  The other replaces a busy-wait loop with a call to
	Delay(), which is a much more environmentally (and battery-) friendly
	option in Advanced Mac Substitute and Mac OS X's Classic (the Blue Box).
	
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

static const UInt16 Lemmings_wait_loop[] =
{
	0x42A7,  // CLR.L    -(A7)
	0xA975,  // _TickCount
	0xBE9F,  // CMP.L    (A7)+,D7
	0x64F8,  // BCC.S    *-6
};

static const UInt16 Lemmings_wait_trap[] =
{
	0x5288,  // ADDQ.L   #1,A0
	0x91F8,  // SUBA.L   0x016A,A0
	0x016A,  //          ^^^^^^
	_Delay,
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
void install_patches( Handle h )
{
	enum
	{
		byte_clear_to_displaced_A5 = 0x422D,  // dst: mode 5, reg 5
		byte_set_T_to_displaced_A5 = 0x50ED,  // dst: mode 5, reg 5
	};
	
	const int query_offset = 0x00537a;
	
	if ( h  &&  GetHandleSize( h ) >= query_offset )
	{
		UInt32* p = (UInt32*) (*h + query_offset);
		
		if ( *p == byte_clear_to_displaced_A5 << 16 | (UInt16) -342 )
		{
			/*
				Hot-patch Lemmings to fix a crash that occurs right at the end
				of a level played with sound -- /before/ you're given the code
				to the next level!
				
				In CODE segment 2, at offset $00536c, there's a function that
				stops the sound engine and frees various resources involved,
				one of which is an array of 104-byte elements.  Another array
				has 38-byte elements.  There's a VBL task struct from (-310,A5)
				to (-296,A5); anything at an address just higher than that can
				be considered VBL payload data, including a flag, the element
				count, and the first array's base address.
				
				Here's what the function does:
				  * sets a flag at (-296,A5)
				  * clears a flag at (-342,A5)
				  * tests an element count at (-294,A5), bailing out if zero
				  * calls a function that marks elements invalid
				  * removes a VBL task via _VRemove
				  * loops over the array, calling a no-op on each element
				  * tests the base address of the array it already looped over
				  * frees the array via _DisposePtr
				  * clears the array pointer variable at (-292,A5)
				  * calls the function at $0051d8
				
				Here's what that last function does:
				  * tests another array base address at (-350,A5)
				  * loops over that array, calling $00510c on each element
				  * frees the array via _DisposePtr
				
				Here's what /that/ function does:
				  * calls $005960
				  * (other stuff that's not relevant)
				
				And finally, here's what **that** function does:
				  * tests the flag at (-342,A5), bailing out if set
				  * loops over the array at (-292,A5), testing each first byte
				
				The VBL task is removed, the first array is destroyed, and the
				application crashes processing the first element of the second
				array, because it dereferences the first byte of the first slot
				of the first array, whose pointer location was already cleared,
				resulting in a NULL dereference.
				
				It's not entirely clear what the exact semantics of the flags
				are, but they appear to be similar.  The VBL payload flag at
				(-296,A5) is invariably cleared on VBL install and set on VBL
				removal -- so it has negative polarity.  It seems to mean 'VBL
				not installed'.  The other flag, at (-342,A5), is set early in
				an initialization routine and reset in the same function after
				VInstall() succeeds.  Most of the subroutines that test it bail
				out if it's set, including one that then sets it and removes
				the VBL.  The one exception bails out if it's clear and then
				immediately clears it, subsequently initializing the first
				array above and installing the VBL.
				
				It's another reversed polarity flag, by all indications except
				one:  The function at $00536c, which calls VRemove(), /clears/
				rather than sets the flag.  If we patch that function to set
				the flag instead, the function at $005960 -- that crashes when
				it accesses NULL -- will bail out instead.  We can't set it to
				$01 in just the two words we have to play with, but we can set
				it to $FF, which is just as good as far as TST is concerned.
				
				Old:  CLR.B    (-342,A5)
				New:  ST.B     (-342,A5)
			*/
			
			UInt16* q = (UInt16*) p;
			
			*q = byte_set_T_to_displaced_A5;
			
			HNoPurge( h );
		}
		
		UInt16* q = (UInt16*) (*h + 0x002aaa);
		
		if ( equal_words( (UInt16*) q, VEC_LEN( Lemmings_wait_loop ) ) )
		{
			my_memcpy( q, Lemmings_wait_trap, sizeof Lemmings_wait_trap );
			
			HNoPurge( h );
		}
	}
}

static
void TEInit_handler()
{
	if ( Handle h = Get1Resource( 'Psyg', 0 ) )
	{
		ReleaseResource( h );
		
		install_patches( Get1Resource( 'CODE', 2 ) );
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
