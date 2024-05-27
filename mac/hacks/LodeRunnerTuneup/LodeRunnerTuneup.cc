/*
	LodeRunnerTuneup.cc
	-------------------
	
	Lode Runner Tune-up INIT for Mac OS
	
	Copyright 2020-2024, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
	This is a pair of application hot patches for Lode Runner.  One of them
	prevents a crash when clicking in the game window due to an unintended
	write to I/O space, affecting Advanced Mac Substitute (which doesn't
	tolerate such nonsense) but also some color Macs.  The other patches
	the four-tone waveform used to play the end-of-level jingles in order
	to match the specified phase, avoiding audible popping on playback.
	
	This extension incorporates the functionality of both LRCrashFix and
	LRMusicFix and supersedes both.
	
*/

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif
#ifndef __TRAPS__
#include <Traps.h>
#endif

// mac-glue-utils
#include "mac_glue/Memory.hh"

// mac-sys-utils
#include "mac_sys/trap_address.hh"


#pragma exceptions off


static inline
void my_memcpy( void* dst, const void* src, long n )
{
	BlockMoveData( src, dst, n );
}

static inline
void my_memmove( void* dst, const void* src, long n )
{
	BlockMoveData( src, dst, n );
}

static UniversalProcPtr old_TEInit;


/*
	This is an application hot patch to prevent a crash in Lode Runner
	due to an unmapped memory access.  The crash won't occur on 68000-based
	Macs (on which there are no invalid memory addresses), but it has been
	observed in both Advanced Mac Substitute and Basilisk II.  Presumably
	it would also occur on at least some MMU-equipped physical machines.
	
	The problem is that Lode Runner's call to FindControl() is followed by
	an instruction that mishandles the result:  Instead of storing it in a
	dummy variable (which is presumably what was intended), it writes the
	word into unmapped memory.
	
	On period Macintosh hardware, this causes no problem because (a) the
	result is never looked at (in either location) so its absence isn't
	missed, and (b) memory accesses never fail on 68000-based Macs.  Since
	$ffae28 is in unmapped I/O space, writes to it are simply ignored.
	
	A subsequent TrackControl() call is also followed by an instruction that
	stores its result (after popping it off the stack), but this one actually
	does write it to the very same dummy variable that the FindControl() call
	missed.
	
	(I'm not sure how this happened; it's almost certainly not a compiler
	bug, seeing how the correct instruction also occurs later in the same
	function, but it seems a rather odd error to make when writing assembly
	language by hand.  Perhaps the binary bitrotted before duplication?)
	
	003afe:  _FindControl
	003b00:  MOVE.W   (SP)+,0xae28       // $31df ae28
	003b04:  BEQ      *+26               // if (Z) goto $003b1e
	...
	003b14:  _TrackControl
	003b16:  MOVE.W   (SP)+,(-20952,A5)  // $3b5f ae28
	003b1a:  BNE      *+8                // if (!Z) goto $003b22
	
	Note that $ae28 and -20952 are the same bit pattern, just disassembled
	differently.  Both instructions correctly pop the result off of the
	stack, but one writes it to an application global (via an A5-relative
	reference) and the other to the absolute address $ffffae28.  Both times,
	the result is checked by using conditional branches, so to that end it
	doesn't matter where the result goes as long as the CCR gets updated.
	
	However, xv68k is rather strict about what address ranges are defined
	for memory access, so clicking Lode Runner's game window writes to
	unmapped memory and halts the emulator.  To avoid that, we'll hot-patch
	the broken instruction.
*/

static inline
void install_hit_test_patch( Handle h, Size handle_size )
{
	enum
	{
		long_pop_to_absolute_short = 0x31DF,  // dst: mode 7, reg 0
		long_pop_to_displaced_A5   = 0x3B5F,  // dst: mode 5, reg 5
		
		// These are offsets relative to the start of the 'CODE' resource.
		
		offset_to_FindControl = 0x3afe,
		minimum_handle_size   = offset_to_FindControl + 4,
	};
	
	if ( h  &&  handle_size > minimum_handle_size )
	{
		UInt16* p = (UInt16*) (*h + offset_to_FindControl);
		
		if ( *p++ == _FindControl  &&  *p == long_pop_to_absolute_short )
		{
			*p = long_pop_to_displaced_A5;
		}
	}
}

/*
	This is a quick and dirty system patch to fix four-tone playback in
	Lode Runner when run in the Advanced Mac Substitute environment.
	
	The problem is that Lode Runner populates the four-tone sound record
	with triangle waves running from 00 to FE and back to 00, but with a
	phase of zero, which corresponds to physical level 00 but logical level
	-128, as opposed to a quarter rotation later, at physical level 80 and
	logical level 0.  The jump from silence at level 0 to the wave start at
	level -128 is audible as a popping noise.
	
	This patch rotates the waveform data in the wave table embedded in the
	code, aligning it to a phase of zero -- which avoids the level jump.
	
	The reported audio defect might not be noticeable with period hardware,
	but it has been observed in emulators including Advanced Mac Substitute.
*/

static inline
void install_waveform_patch( Handle h, Size handle_size )
{
	/*
		Lode Runner defines a triangle wave like this:
		
			           /\
			          /  \
			         /    \
			        /      \
		
		(Each segment above represents 32 bytes of a 256-byte waveform.)
		
		That would be fine if it set the phase to 64 (i.e. at pi/2).
		But it sets the phase to 0, resulting in popping when a voice enters.
		
		Here's the same diagram with the preceding silence marked:
		
			+          /\
			________  /  \
			         /    \
			-       /      \
		
		The Y axis represents wave amplitude.  Notice the jump from Y=0
		(indicating zero amplitude, i.e. silence) to the wave's nadir.
		This is audible as a pop during playback.
		
		We'll rotate the waveform to resemble a sine wave rather than
		an inverse cosine wave (and thereby match the phase of 0):
		
			+        /\
			________/  \
			            \  /
			-            \/
		
		This eliminates the signal jump (and consequently the resulting pop).
	*/
	
	enum
	{
		// These are offsets relative to the start of the 'CODE' resource.
		
		offset_to_wavetable = 0x29fc,
		offset_to_wave_LEA  = 0x6c7a,  // LEA *-17022,A0    ; $0029fc
		offset_to_wave_disp = offset_to_wave_LEA + 2,
		minimum_handle_size = offset_to_wave_LEA + 4,
	};
	
	if ( h  &&  handle_size > minimum_handle_size )
	{
		Ptr p = *h;
		
		// Verify that the wavetable is referenced by the code.
		
		const short displacement = *(short*) &p[ offset_to_wave_disp ];
		
		if ( offset_to_wave_disp + displacement == offset_to_wavetable )
		{
			Ptr wave = p + offset_to_wavetable;
			
			/*
				Rotate the wave, in effect removing the first 64 bytes
				and reinserting them at the end.
				
				Temp:   Wave:
				
				           /\
				          /  \
				         /    \
				        /      \
				        
				           /\
				          /  \
				 /       /    \
				/       /      \
				        
				         /\
				        /  \
				 /          \ \
				/            \ \
				        
				         /\
				        /  \
				 /          \  /
				/            \/
				
			*/
			
			Byte temp[ 64 ];
			
			my_memcpy ( temp,       wave,       64 );
			my_memmove( wave,       wave + 64, 192 );
			my_memcpy ( wave + 192, temp,       64 );
		}
	}
}

static
void TEInit_handler()
{
	if ( Handle h = GetResource( 'GLEN', 0 ) )
	{
		ReleaseResource( h );
		
		if ( (h = GetResource( 'CODE', 1 )) )
		{
			Size size = mac::glue::GetHandleSize_raw( h );
			
			install_hit_test_patch( h, size );
			install_waveform_patch( h, size );
		}
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
