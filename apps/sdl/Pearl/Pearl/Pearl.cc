/*
	Pearl.cc
	--------
*/

// Mac OS X
#ifdef __APPLE__
#include <CoreFoundation/CFBundle.h>
#endif

// POSIX
#include <signal.h>
#include <unistd.h>

// Standard C
#include <errno.h>
#include <stdlib.h>

// libsdl2
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>

// frontend-common
#include "frend/cursor.hh"
#include "frend/tempfile.hh"
#include "frend/update_fifo.hh"

// libsndtrack
#include "event_loop.hh"

// log-of-war
#include "logofwar/report.hh"

// posix-utils
#include "posix/bindir.hh"

// rasterlib
#include "raster/raster.hh"

// v68k-cursor
#include "cursor/cursor.hh"

// Pearl
#include "Pearl/audio.hh"
#include "Pearl/blitter.hh"
#include "Pearl/cursor.hh"
#include "Pearl/events.hh"
#include "Pearl/screen.hh"
#include "Pearl/window.hh"


#define PROGRAM  "Pearl"


#ifdef __APPLE__

static
CFBundleRef get_main_bundle()
{
	return CFBundleGetMainBundle();
}

static
CFStringRef get_bundle_string( CFBundleRef bundle, CFStringRef key )
{
	return (CFStringRef) CFBundleGetValueForInfoDictionaryKey( bundle, key );
}

static
CFIndex get_utf8_string_size( CFIndex length )
{
	return CFStringGetMaximumSizeForEncoding( length, kCFStringEncodingUTF8 );
}

static
CFIndex get_utf8_string_size( CFStringRef str )
{
	return get_utf8_string_size( CFStringGetLength( str ) );
}

static
void set_app_utf8_string( CFStringRef key, void (*setter)( const char* ) )
{
	CFBundleRef main_bundle = get_main_bundle();

	if ( main_bundle == NULL )
	{
		return;
	}

	CFStringRef str = get_bundle_string( main_bundle, key );

	if ( str == NULL )
	{
		return;
	}

	const char* str_utf8 = CFStringGetCStringPtr( str, kCFStringEncodingUTF8 );

	if ( str_utf8 == NULL )
	{
		const CFIndex buffer_size = get_utf8_string_size( str );

		char* utf8_buffer = (char*) alloca( buffer_size );

		if ( CFStringGetCString( str, utf8_buffer, buffer_size, kCFStringEncodingUTF8 ) )
		{
			str_utf8 = utf8_buffer;
		}
	}

	setter( str_utf8 );
}

static
void set_app_name( void (*setter)( const char* ) )
{
	set_app_utf8_string( CFSTR( "CFBundleName" ), setter );
}

#else

static
void set_app_name( void (*setter)( const char* ) )
{
	setter( PROGRAM );
}

#endif

#if ! defined(__MSYS__)  &&  ! defined(__CYGWIN)

static
void sigchld( int )
{
	frend::unblock_update_waiters();
}

#endif

int run_event_loop( const raster::raster_load& load, const raster::raster_desc& desc )
{
	using namespace Pearl;

	const int width  = desc.width;
	const int height = desc.height;
	
	Window window( SDL_GetHint( SDL_HINT_APP_NAME ), width, height );
	if ( window == NULL )
	{
		FATAL = "could not create window";
		return 1;
	}

	Blitter blitter( window );
	blitter.prep( desc.stride, width, height, desc.weight );

	SDL_Point cursor_limit = { width - 1, height - 1 };
	SDL_Point initial_cursor_location = { 15, 15 };
	Cursor cursor( cursor_limit, initial_cursor_location, blitter );
	cursor.set_composite_mode( Cursor::CompositeMode_host_OS );

	bool running = true;
	SDL_Event event;
	while ( running )
	{
		if ( ! SDL_WaitEvent( &event ) )
		{
			continue;
		}

		if ( event.type == pearl_event_class )
		{
			SDL_UserEvent* pearl_event = (SDL_UserEvent*) &event;
			switch ( pearl_event->code )
			{
				case kEventPearlScreenBits:
					continue;

				case kEventPearlScaleMultiple:
					window.scale( *(float*) pearl_event->data1 );
					continue;

				case kEventPearlIntegerScale:
					blitter.toggle_integer_scaling();
					continue;

				case kEventPearlFullscreen:
					window.toggle_fullscreen();
					cursor.move_to( cursor.get_last_location() );
					continue;

				case kEventPearlKeyboardGrab:
					window.toggle_keyboard_grab();
					continue;

				case kEventPearlMouseGrab:
					window.toggle_mouse_grab();
					continue;

				default:
					break;
			}

			if ( cursor.get_composite_mode() != Cursor::CompositeMode_software )
			{
				using frend::cursor_state;
				using frend::shared_cursor_state;

				if ( const shared_cursor_state* latest_cursor = cursor_state )
				{
					if ( pearl_event->code == kEventPearlCursorBits )
					{
						cursor.set( *latest_cursor );
					}

					cursor.show( latest_cursor->visible );
				}
			}
				
			window.update( load, desc, blitter );
			continue;
		}

		if (           event.type == SDL_WINDOWEVENT &&
		     ( event.window.event == SDL_WINDOWEVENT_RESIZED ||
		       event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED ) )
		{
			blitter.size( event.window.data1, event.window.data2 );
			continue;
		}

		running = handle_sdl_event( event, cursor );
	}

	return 0;
}

static
void set_sdl_app_name( const char* app_name )
{
	SDL_SetHint( SDL_HINT_APP_NAME, app_name );
}

int main( int argc, char** argv )
{
	using frend::tempfile_location;

	set_app_name( &set_sdl_app_name );

#if ! defined(__MSYS__)  &&  ! defined(__CYGWIN)

	signal( SIGCHLD, &sigchld );

#endif

	// Reserve fds 6 and 7 for graft to connect ams-fs/xv68k to freemountd.
	dup2( STDERR_FILENO, 6 );
	dup2( STDERR_FILENO, 7 );

	if ( SDL_Init( SDL_INIT_VIDEO ) != 0 )
	{
		FATAL = "could not initialize SDL";
		return 1;
	}

	using namespace Pearl;

	const char* works_path = tempfile_location();
	int nok = mkdir( works_path, 0777 );
	if ( nok  &&  errno != EEXIST )
	{
		FATAL = "could not create temporary directory";
		return 1;
	}

	int bindir_fd = bindir( argv[ 0 ] );
	chdir( works_path );

	// Create the sound fds before launching the coprocess.
	audio_stream audio;

	try
	{
		emulated_screen screen( bindir_fd, works_path );
		close( bindir_fd );

		audio.start();

		const raster_load& load = screen.load();
		const raster_desc& desc = screen.desc();

		if ( run_event_loop( load, desc ) != 0 )
		{
			WARNING = "could not run event loop";
		}

		close( events_fd );
	}
	catch ( ... )
	{
		FATAL = "could not run emulator process";
	}

	// Stop audio after the coprocess has exited.
	// Now we can close the sound fd, unblocking libsndtrack's event loop.
	audio.stop();

	rmdir( works_path );

	return 0;
}
