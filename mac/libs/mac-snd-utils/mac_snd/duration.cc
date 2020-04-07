/*
	duration.cc
	-----------
*/

#include "mac_snd/duration.hh"

// Standard C
#include <stddef.h>

// SoundDriver
#include "SoundDriver/SoundDriver.h"


namespace mac {
namespace snd {

static
unsigned long long nanoseconds_from_ticks( unsigned ticks )
{
	const int ns_per_sec = 1000 * 1000 * 1000;
	
	return ticks * (ns_per_sec * 100ull) / 6015;
}

static
long long duration( const Tone* tones, unsigned count )
{
	unsigned ticks = 0;
	
	const Tone* tone = tones;
	
	/*
		`count` is a multiple of the wavelength.  A zero wavelength is invalid,
		regardless of what Inside Macintosh says.  So `count` can only be zero
		when it's a terminator.
	*/
	
	do
	{
		ticks += tone->duration;
	}
	while ( tone++->count != 0 );
	
	return nanoseconds_from_ticks( ticks );
}

static inline
long long duration( const SWSynthRec& rec, unsigned size )
{
	return duration( rec.triplets, size / 6 );
}

static inline
long long duration( const FFSynthRec& rec, unsigned count )
{
	/*
		`count` is the number of samples.
		
		`rec.count` is the (possibly fractional) number of samples we advance
		every 44.93 microseconds, expressed as a 32-bit fixed-point value.
		The actual fraction in floating point would be `rec.count / 65536.0`.
		Call this the playback rate.
		
		If the playback rate is one, then the duration is 44.93us per sample.
		If the playback rate is two, then we skip a sample each time, and the
		duration is halved.  Conversely, if the playback rate is 1/2, each
		sample is used twice, and the duration is doubled.  Since a rate is a
		number of things per time unit, whereas we want its reciprocal (an
		amount of time for something), we have to /divide/ by the rate to get
		a duration.
	*/
	
	return count * 44930ull * 65536 / rec.count;
}

static inline
long long duration( const FTSoundRec& rec )
{
	return nanoseconds_from_ticks( rec.duration );
}

static inline
long long duration( const FTSynthRec& rec )
{
	return duration( *rec.sndRec );
}

long long duration( const void* buffer, unsigned size )
{
	if ( size < 6 )
	{
		return -1;
	}
	
	const short swOffset = offsetof( SWSynthRec, triplets  );
	const short ffOffset = offsetof( FFSynthRec, waveBytes );
	
	const short mode = *(const short*) buffer;
	
	switch ( mode )
	{
		case -1:  return duration( *(SWSynthRec*) buffer, size - swOffset );
		case  0:  return duration( *(FFSynthRec*) buffer, size - ffOffset );
		case  1:  return duration( *(FTSynthRec*) buffer );
		
		default:
			return -1;
	}
}

}
}
