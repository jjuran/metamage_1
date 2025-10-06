/*
	async.cc
	--------
*/

#include "async.hh"

// Mac OS
#ifndef __DEVICES__
#include <Devices.h>
#endif
#ifndef __FIXMATH__
#include <FixMath.h>
#endif

// log-of-war
#include "logofwar/report.hh"

// ams-common
#include "callouts.hh"

// ams-snd
#include "buffers.hh"


enum
{
	/*
		Apparently noHardwareErr isn't used for anything -- at least
		nothing deserving of mention in Apple's Sound Manager 3.3
		documentation -- so we'll use it here for things that are
		yet to be implemented.  (There's also unimpErr, but that's
		used to indicate that a particular software operation is
		unsupported by a hardware device, e.g. volume control.)
	*/
	
	unimplemented = -200,  // same as noHardwareErr
};

static inline
Fixed playback_rate_from_sample_rate( UnsignedFixed sampleRate )
{
	enum
	{
		rate22khz = 0x56EE8BA3,
		rate11khz = 0x2B7745D1,
	};
	
	switch ( sampleRate )
	{
		case rate22khz:  return 0x00010000;
		case rate11khz + 1:  // TaskMaker does this
		case rate11khz:  return 0x00008000;
		default:         return FixDiv( sampleRate, rate22khz );
	}
}

OSErr play_async( SndChannel* chan, Ptr p, long n_samples, UFixed rate )
{
	Byte* samples = (Byte*) p;
	
	Size samples_remaining = n_samples;
	Size payload_len       = n_samples;
	
	Fixed playback_rate = playback_rate_from_sample_rate( rate );
	
	if ( payload_len > 30000 )
	{
		if ( playback_rate != 0x00010000  &&  playback_rate != 0x00008000 )
		{
			ERROR = "sampled sound size of ", payload_len, " bytes is too long";
			
			return unimplemented;
		}
		
		payload_len = 370 * 81;  // 29970
	}
	
	OSErr err;
	
	do
	{
		audio_buffer* buffer = alloc_buffer();
		
		if ( buffer == NULL )
		{
			ERROR = "bufferCmd: audio buffers exhausted";
			
			return notEnoughBufferSpace;
		}
		
		buffer->ch       = chan;
		buffer->ff.count = playback_rate;
		
		fast_memcpy( buffer->ff.waveBytes, samples, payload_len );
		
		samples           += payload_len;
		samples_remaining -= payload_len;
		
		ParamBlockRec& pb = buffer->pb;
		IOParam&       io = pb.ioParam;
		
		io.ioPosOffset = samples_remaining;
		io.ioReqCount  = 6 + payload_len;
		
		err = PBWriteAsync( &pb );
		
		if ( samples_remaining < payload_len )
		{
			payload_len = samples_remaining;
		}
	}
	while ( samples_remaining > 0  &&  err == noErr );
	
	return err;
}
