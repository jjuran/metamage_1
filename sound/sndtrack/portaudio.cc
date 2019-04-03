/*
	portaudio.cc
	------------
*/

#include "portaudio.hh"

// Standard C
#include <string.h>

// PortAudio
#include "portaudio.h"

// sndtrack
#include "exceptions.hh"
#include "synthesize.hh"


namespace portaudio
{

const int channel_count = 1;  // monophonic
const int sample_size   = 1;  // 8-bit

const int ground = 0x80;  // unsigned 8-bit samples are centered at 128

const int sample_rate = 22256;

const int frames_per_buffer = 370;

const int frame_size = sample_size * channel_count;
const int buffer_size = frame_size * frames_per_buffer;


static
int stream_callback( const void*                      input,
                     void*                            output,
                     unsigned long                    framesPerBuffer,
                     const PaStreamCallbackTimeInfo*  timeInfo,
                     PaStreamCallbackFlags            statusFlags,
                     void*                            userData )
{
	short count = synthesize( *(sample_buffer*) output );
	
	if ( short n_unset = frames_per_buffer - count )
	{
		char* gap = (char*) output + count;
		
		memset( gap, ground, n_unset );  // fill gap with silence
	}
	
	return paContinue;
}

static PaStreamParameters output_parameters;
static PaStream*          output_stream;

class PortAudio
{
	private:
		// non-copyable
		PortAudio           ( const PortAudio& );
		PortAudio& operator=( const PortAudio& );
	
	public:
		PortAudio();
		
		~PortAudio();
};

PortAudio::PortAudio()
{
	PaError error;
	
	if (( error = Pa_Initialize() ))
	{
		take_exception( audio_platform_start_error );
		return;
	}
	
	PaDeviceIndex device = Pa_GetDefaultOutputDevice();
	
	if ( device == paNoDevice )
	{
		take_exception( audio_platform_start_error );
		return;
	}
	
	const PaDeviceInfo* device_info = Pa_GetDeviceInfo( device );
	
	output_parameters.device = device;
	output_parameters.channelCount = channel_count;
	output_parameters.sampleFormat = paUInt8;
	output_parameters.suggestedLatency = device_info->defaultLowOutputLatency;
	
	error = Pa_OpenStream( &output_stream,
	                       NULL,
	                       &output_parameters,
	                       sample_rate,
	                       frames_per_buffer,
	                       paNoFlag,
	                       &stream_callback,
	                       NULL );
	
	if ( error )
	{
		take_exception( audio_platform_start_error );
	}
}

PortAudio::~PortAudio()
{
	if ( output_stream )
	{
		if ( Pa_CloseStream( output_stream ) != paNoError )
		{
			take_exception( audio_platform_stop_error );
		}
	}
	
	Pa_Terminate();
}

bool start()
{
	static PortAudio portaudio;
	
	const bool ok = Pa_StartStream( output_stream ) == paNoError;
	
	if ( ! ok )
	{
		take_exception( audio_platform_start_error );
	}
	
	return ok;
}

bool stop()
{
	const bool ok = Pa_StopStream( output_stream ) == paNoError;
	
	if ( ! ok )
	{
		take_exception( audio_platform_stop_error );
	}
	
	return ok;
}

}  // namespace portaudio
