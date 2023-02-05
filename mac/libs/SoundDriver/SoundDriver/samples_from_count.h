/*
	samples_from_count.hh
	---------------------
	
	SoundDriver
	
	Copyright 2024, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
	SoundDriver is a library for calling and implementing
	the classic Mac OS Sound Driver API.
	
*/

#ifndef SOUNDDRIVER_SAMPLESFROMCOUNT_H
#define SOUNDDRIVER_SAMPLESFROMCOUNT_H


#ifdef __MC68K__

inline
asm
long samples_from_count( unsigned short count : __D0 )
{
	machine 68020
	
	EXT.L    D0
	SWAP     D0
	
	MOVE.L   #10000000,D1   // (1,000,000,000 / 100)
	MOVE.L   #351963648,D2  // (4493 * 78336)
	
	MULU.L   D1,D1:D0
	DIVU.L   D2,D1:D0
}

#else

inline
long samples_from_count( unsigned short count )
{
	const int shift = 16;
	
	const long long ns_per_sec = 1000 * 1000 * 1000;
	
	/*
		The wave's "frequency" (half-cycles per second) is (783360 / count) Hz,
		according to Inside Macintosh, Volume II.  (They mean half frequency.)
		Therefore a half-period is (count / 783360) seconds long.
		
		(Note that 783360 is a multiple of 256.  So we can think of `count` as
		being in 8.8 fixed-point format, in which case a half-period duration
		is (count / 3060) seconds and a full period is (count / 1530) seconds.)
		
		The duration of a sample is 44.93us == (44930 / 10^9)s.
		
		Dividing the half-period duration by the sample duration yields the
		number of samples per half-period: (count / 783360) / (44930 / 10^9)
		
		Multiplying numerator and denominator by (783360 * 10^9) yields
		(count * 10^9) / (44930 * 783360).  We'll also shift left to preserve
		some fraction bits.
		
		Divide `ns_per_sec` and the (44930 * 783360) divisor by 100 so the
		latter fits into 32 bits.
		
	*/
	
	return ((count << shift) * (ns_per_sec / 100)) / (4493 * 78336);
}

#endif

#endif

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
