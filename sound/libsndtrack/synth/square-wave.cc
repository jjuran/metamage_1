/*
	square-wave.cc
	--------------
*/

#include "synth/square-wave.hh"

// Standard C
#include <stdint.h>
#include <string.h>

// SoundDriver
#include "SoundDriver/SoundDriver.h"

// libsndtrack
#include "output.hh"


static inline
bool end_of_tones( const Tone* tone )
{
	/*
		Inside Macintosh, Volume II documents that a Tones array is terminated
		by an element whose three fields are all zero.  It also documents that
		`count` is valid for the entire range of 0 - 65535 inclusive, although
		it offers no insight as to what a count of 0 means or even if it makes
		sense at all, or for that matter a guarantee that at the very least it
		won't crash the OS.
		
		Rather than add a separate zero-divisor check, we'll craft one here by
		discarding the checks of the other two fields.  In other words, we now
		assume that a zero-count Tone is never passed as data and therefore is
		only used as an end-of-data marker.
	*/
	
	return tone->count == 0;
	
	// not reached
	return tone->count == 0  && tone->amplitude == 0  &&  tone->duration == 0;
}

static inline
size_t min( size_t a, size_t b )
{
	return b < a ? b : a;
}

const int shift = 16;

static inline
Fixed samples_from_count( uint16_t count )
{
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

static Tone* tone;
static Fixed demiperiod_samples;
static Fixed remaining_run_length;

static int sample;

static uint16_t last_count;

short sw_synth( sample_buffer& output, sw_buffer& rec, bool reset )
{
	if ( reset )
	{
		tone = rec.triplets;
	}
	
	while ( tone->duration <= 0 )
	{
		if ( end_of_tones( ++tone ) )
		{
			return -1;
		}
		
		reset = true;
	}
	
	if ( reset )
	{
		/*
			Square wave tones are too loud relative to four-tone and
			freeform-wave sounds (as well as the host system generally).
			
			Halve their amplitude to compensate.
		*/
		
		const uint16_t amplitude = tone->amplitude / 2;
		
		const int crest = 128 + amplitude / 2;
		const int nadir = 128 - amplitude / 2;
		
		if ( tone->count == last_count )
		{
			sample = (int8_t) sample < 0 ? crest : nadir;
		}
		else
		{
			last_count = tone->count;
			
			// Start by taking the wave low.  Real Macs appear to do this.
			
			sample = nadir;
			
			demiperiod_samples = samples_from_count( tone->count );
			
			remaining_run_length = demiperiod_samples;
		}
	}
	
	--tone->duration;
	
	if ( demiperiod_samples == 0 )
	{
		return 0;  // emit silence when tone->count == 0
	}
	
	output_sample_t* p = output.data;
	
	size_t n_samples_to_fill = samples_per_buffer;
	
	goto start;
	
	do
	{
		/*
			A real Mac alternates between the full amplitude and zero, rather
			than between two levels equidistant from ground (128).  AC coupling
			electrically removes the inherent bias from the output signal, but
			since we don't have that, we have to do it explicitly -- lest our
			square wave sounds be riddled with clicks and thumps.
		*/
		
		sample = -sample;  // same low byte as (256 - sample)
		
	start:
		
		size_t samples_in_run = remaining_run_length >> shift;
		
		if ( samples_in_run == 0 )
		{
			remaining_run_length += demiperiod_samples;
			
			samples_in_run = remaining_run_length >> shift;
		}
		
		size_t n_samples = min( n_samples_to_fill, samples_in_run );
		
		memset( p, sample ^ 0x80, n_samples * sizeof (output_sample_t) );
		
		p += n_samples;
		
		n_samples_to_fill    -= n_samples;
		remaining_run_length -= n_samples << shift;
	}
	while ( n_samples_to_fill );
	
	return samples_per_buffer;
}
