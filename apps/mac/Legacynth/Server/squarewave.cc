/*
	squarewave.cc
	-------------
	
	Legacynth Server for classic Mac OS
	
	Copyright 2024, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
	Legacynth implements the Sound Driver API in traditional Mac OS
	environments that don't.
	
	This application is a sound server and API adapter, converting
	I/O requests to the Sound Driver into Sound Manager commands.
	
	This module implements the square-wave synthesizer.
	
*/

#include "squarewave.hh"

// SoundDriver
#include "SoundDriver/samples_from_count.h"
#include "SoundDriver/SoundDriver.h"

// LegacynthServer
#include "intrinsics.hh"


const int shift = 16;

static inline
UInt32 min( UInt32 a, UInt32 b )
{
	return b < a ? b : a;
}

static inline
char* mempset( Byte byte, char* dst, UInt32 n )
{
	while ( n-- > 0 )
	{
		*dst++ = byte;
	}
	
	return dst;
}

long squarewave_fill( const Tone* tone, Ptr last, Ptr p, long n )
{
	while ( n >= 370  &&  (Ptr) tone <= last  &&  tone->count != 0 )
	{
		UInt32 n_samples_to_fill = min( n, MULU( tone->duration, 370 ) );
		
		n -= n_samples_to_fill;
		
		const UInt32 amplitude = tone->amplitude / 2;
		
		const int crest = 128 + amplitude / 2;
		const int nadir = 128 - amplitude / 2;
		
		int sample = crest;
		
		const Fixed demiperiod_samples = samples_from_count( tone->count );
		
		Fixed remaining_run_length = 0;
		
		do
		{
			sample = -sample;
			
			remaining_run_length += demiperiod_samples;
			
			UInt32 samples_in_run = remaining_run_length >> shift;
			
			UInt32 n_samples = min( n_samples_to_fill, samples_in_run );
			
			p = mempset( sample ^ 0x80, p, n_samples );
			
			n_samples_to_fill    -= n_samples;
			remaining_run_length -= n_samples << shift;
		}
		while ( n_samples_to_fill );
		
		++tone;
	}
	
	return n;
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
