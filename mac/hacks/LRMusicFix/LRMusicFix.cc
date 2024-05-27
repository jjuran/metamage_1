/*
	LRMusicFix.cc
	-------------
	
	Lode Runner Music Fix INIT for Mac OS
	
	Copyright 2020-2024, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
	This is a quick and dirty system patch to fix four-tone playback in
	Lode Runner when run in the Advanced Mac Substitute environment.
	
	The problem is that Lode Runner populates the four-tone sound record
	with triangle waves running from 00 to FE and back to 00, but with a
	phase of zero, which corresponds to physical level 00 but logical level
	-128, as opposed to a quarter rotation later, at physical level 80 and
	logical level 0.  The jump from silence at level 0 to the wave start at
	level -128 is audible as a popping noise.
	
	This program rotates the waveform data in the wave table embedded in
	the code, aligning it to a phase of zero, which avoids the level jump.
	
	The reported audio defect might not be noticeable with period hardware,
	but it has been observed in emulators including Advanced Mac Substitute.
	
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
