/*
	SoundManager.cc
	---------------
*/

#include "SoundManager.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __SOUND__
#include <Sound.h>
#endif

// mac-config
#include "mac_config/upp-macros.hh"

// libsndtrack
#include "exceptions.hh"
#include "output.hh"
#include "recording.hh"
#include "synthesize.hh"


namespace SoundManager
{

const int buffer_size = frame_size * frames_per_buffer;


static SndChannelPtr  channel;
static ExtSoundHeader header[ 2 ];
static Ptr            buffer;
static int            selector = 0;

static bool running;

static
OSErr queue_next()
{
	ExtSoundHeader& sh = header[ selector ];
	
	Ptr p = buffer + buffer_size * selector;
	
	sh.samplePtr   = p;
	sh.numChannels = 1;
	sh.sampleRate  = rate22khz;
	sh.encode      = extSH;
	sh.numFrames   = 370;
	sh.sampleSize  = 16;
	
	selector = ! selector;
	
	OSErr err = noErr;
	
	if ( synthesize( p )  ||  running )
	{
		append_to_recording( p, buffer_size );
	
		SndCommand playback = { bufferCmd,   0, (long) &sh };
		SndCommand callback = { callBackCmd, 0, 0 };
		
		(err = SndDoCommand( channel, &playback, true ))  ||
		(err = SndDoCommand( channel, &callback, true ));
	}
	
	return err;
}

static
pascal void sound_callback( SndChannelPtr channel, SndCommand* command )
{
	queue_next();
}

DEFINE_UPP( SndCallBack, sound_callback )

class SoundManager_client
{
	private:
		// non-copyable
		SoundManager_client           ( const SoundManager_client& );
		SoundManager_client& operator=( const SoundManager_client& );
	
	public:
		SoundManager_client();
		
		~SoundManager_client();
};

static
OSErr Sound_startup()
{
	OSErr err;
	
	if ( ! channel )
	{
		err = SndNewChannel( &channel,
		                     sampledSynth,
		                     initNoDrop | initMono,
		                     UPP_ARG( sound_callback ) );
		
		if ( err )
		{
			return err;
		}
	}
	
	buffer = NewPtr( buffer_size * 2 );
	
	if ( ! buffer )
	{
		return memFullErr;
	}
	
	return noErr;
}

SoundManager_client::SoundManager_client()
{
	if ( OSErr err = Sound_startup() )
	{
		take_exception( audio_platform_start_error );
	}
}

static
OSErr stop_sound()
{
	running = false;
	
	OSErr err = noErr;
	
	if ( channel )
	{
		err = SndDisposeChannel( channel, false );
		
		channel = NULL;
	}
	
	return err;
}

SoundManager_client::~SoundManager_client()
{
	if ( OSErr err = stop_sound() )
	{
		take_exception( audio_platform_stop_error );
	}
}

bool start()
{
	static SoundManager_client client;
	
	running = true;
	
	return queue_next();
}

bool stop()
{
	OSErr err = stop_sound();
	
	if ( err )
	{
		take_exception( audio_platform_stop_error );
	}
	
	return err == noErr;
}

}  // namespace SoundManager
