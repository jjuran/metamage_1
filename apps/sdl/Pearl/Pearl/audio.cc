/*
	audio.cc
	--------
*/

#include "Pearl/audio.hh"

// POSIX
#include <unistd.h>

// Standard C
#include <signal.h>
#include <stdlib.h>

// libsdl2
#if defined(__MSYS__) || defined(__CYGWIN__)
// MSYS/Cygwin doesn't know about _beginthreadex / _endthreadex
#define SDL_beginthread NULL
#define SDL_endthread NULL
#endif
#include <SDL2/SDL.h>

// gear
#include "gear/inscribe_decimal.hh"

// libsndtrack
#include "event_loop.hh"
#include "exceptions.hh"
#include "output.hh"
#include "synthesize.hh"


namespace Pearl
{

static const int sample_rate = 7833600 / 352;  // 22254

static SDL_AudioDeviceID device_id = 0;
static SDL_Thread* thread = NULL;

static const char sound_fd_envvar[] = "AMS_SOUND_FD";
static int pipe_fds[ 2 ];

static
void stream_callback( void*, Uint8* stream, int len )
{
	if ( len == samples_per_buffer * sizeof (output_sample_t) )
	{
		synthesize( stream );
	}
}

audio_stream::audio_stream()
{
	if ( device_id != 0 )
	{
		take_exception( audio_platform_start_error );
		return;
	}

	if ( getenv( sound_fd_envvar ) != NULL )
	{
		return;
	}

	if ( SDL_InitSubSystem( SDL_INIT_AUDIO ) != 0 )
	{
		take_exception( audio_platform_start_error );
		return;
	}

	SDL_AudioSpec spec =
	{
		.freq     = sample_rate,
		.format   = AUDIO_S16SYS,
		.channels = channel_count,
		.samples  = 370,
		.callback = stream_callback,
		.userdata = NULL,
	};

	device_id = SDL_OpenAudioDevice( NULL, 0, &spec, NULL, 0 );

	if ( device_id == 0 )
	{
		SDL_QuitSubSystem( SDL_INIT_AUDIO );
		take_exception( audio_platform_start_error );
		return;
	}

	if ( pipe( pipe_fds ) )
	{
		take_exception( audio_platform_start_error );
		return;
	}

	const char* pipe_fd = gear::inscribe_unsigned_decimal( pipe_fds[ 1 ] );

	setenv( sound_fd_envvar, pipe_fd, 0 );
}

audio_stream::~audio_stream()
{
	stop();

	close( pipe_fds[ 0 ] );
	close( pipe_fds[ 1 ] );

	if ( device_id != 0 )
	{
		SDL_CloseAudioDevice( device_id );
	}

	SDL_QuitSubSystem( SDL_INIT_AUDIO );
}

static
bool resume_stream()
{
	if ( device_id == 0 )
	{
		return false;
	}

	SDL_PauseAudioDevice( device_id, SDL_FALSE );

	return true;
}

static
bool pause_stream()
{
	if ( device_id == 0 )
	{
		return false;
	}

	SDL_PauseAudioDevice( device_id, SDL_TRUE );

	return true;
}

static
int thread_entry( void* )
{
	event_loop( pipe_fds[ 0 ], &resume_stream, &pause_stream );

	return 0;
}

bool audio_stream::start()
{
	if ( device_id == 0  ||  thread != NULL )
	{
		return false; 
	}

	close( pipe_fds[ 1 ] );

	thread = SDL_CreateThread( thread_entry, NULL, NULL );

	return true;
}

bool audio_stream::stop()
{
	if ( thread == NULL )
	{
		return false;
	}

	interrupt( SIGINT );

	SDL_WaitThread( thread, NULL );
	thread = NULL;

	return true;
}

}
