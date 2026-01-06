/*
	fourtone.cc
	-----------
	
	Legacynth Server for classic Mac OS
	
	Copyright 2024-2026, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
	Legacynth implements the Sound Driver API in traditional Mac OS
	environments that don't.
	
	This application is a sound server and API adapter, converting
	I/O requests to the Sound Driver into Sound Manager commands.
	
*/

#include "fourtone.hh"

// SoundDriver
#include "SoundDriver/SoundDriver.h"

// LegacynthServer
#include "intrinsics.hh"


struct sample_buffer
{
	Byte data[ 370 ];
	short reserved;
};

static inline
void clear( sample_buffer& buffer )
{
	UInt32* p = (UInt32*) buffer.data;
	
	for ( int i = 0;  i < sizeof buffer / 4;  ++i )
	{
		*p++ = 0;
	}
}

static inline
SInt8 fourtone_sample( Wave wave, Fixed index )
{
	UInt8 i = index >> 16;
	UInt8 j = i + 1;
	
	SInt8 a = wave[ i ] ^ 0x80;
	SInt8 b = wave[ j ] ^ 0x80;
	
	UInt16 p1 = index;
	UInt16 p0 = -p1;
	
	if ( p1 == 0 )
	{
		return a;  // index is exactly on a byte boundary
	}
	
	/*
		The phase marker is between byte boundaries, so the "current" byte is
		really a combination of two consecutive samples.  Proportionally blend
		the two samples instead of taking just the one at the truncated index.
		
		This primarily affects notes with a sample playback rate under 1.0 --
		when two successive indices truncate to the same value, the sample gets
		repeated.  This creates a stairstep pattern in the waveform which is
		audible as a high-pitched tone.  Instead, proportionally blending the
		current sample with the next interpolates a new sample that lies along
		the wave, eliminating this stairstepping and the resulting artifact.
		(Other artifacts still remain, but they're much less pronounced.)
	*/
	
	SInt32 sum = MULS( a, p0 >> 1 )
	           + MULS( b, p1 >> 1 );
	
	return SWAP( sum << 1 );
}

static inline
void fill_from_wavetable( sample_buffer& b, Wave wave, Fixed& si, Fixed rate )
{
	if ( rate == 0 )
	{
		clear( b );
		return;
	}
	
	Byte* p = b.data;
	
	for ( int i = 0;  i < 370;  ++i )
	{
		*p++ = fourtone_sample( wave, si );
		
		si += rate;
	}
}

static
void diminish( sample_buffer& output )
{
	Byte* data = output.data;
	
	for ( short i = 1;  i < 256;  ++i )
	{
		data[ i ] = (SInt8) data[ i ] * (256 - i) / 256;
	}
	
	for ( short i = 256;  i < 370;  ++i )
	{
		data[ i ] = 0;
	}
}

void fourtone_fill( FTSoundRec* sndRec, ToneSpec* prev, short* p, bool last )
{
	short duration = sndRec->duration;
	
	const ToneSpec* current = (const ToneSpec*) &sndRec->sound1Rate;
	
	if ( duration >= 0 )
	{
		Byte** wave = &sndRec->sound1Wave;
		
		sample_buffer cbuf[ 4 ];  // per-channel buffer
		
		for ( int i = 0;  i < 4;  ++i )
		{
			Fixed rate = current[ i ].rate;
			
			if ( rate == 0  ||  last )
			{
				rate = prev[ i ].rate;
			}
			
			Fixed si = prev[ i ].phase;
			
			if ( ! prev[ i ].rate )
			{
				si = SWAP( current[ i ].phase );
			}
			
			fill_from_wavetable( cbuf[ i ], wave[ i ], si, rate );
			
			if ( prev[ i ].rate  &&  ! current[ i ].rate )
			{
				diminish( cbuf[ i ] );
			}
			
			prev[ i ].rate  = current[ i ].rate;
			prev[ i ].phase = si;
		}
		
		for ( short i = 0;  i < 370;  ++i )
		{
			SInt8 s1 = cbuf[ 0 ].data[ i ];
			SInt8 s2 = cbuf[ 1 ].data[ i ];
			SInt8 s3 = cbuf[ 2 ].data[ i ];
			SInt8 s4 = cbuf[ 3 ].data[ i ];
			
			*p++ = (s1 + s2 + s3 + s4) << 6;
		}
	}
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
