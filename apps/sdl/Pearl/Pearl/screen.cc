/*
	screen.cc
	---------
*/

#include "Pearl/screen.hh"

// libsdl2
#if defined(__MSYS__) || defined(__CYGWIN__)
// MSYS/Cygwin doesn't know about _beginthreadex / _endthreadex
#define SDL_beginthread NULL
#define SDL_endthread NULL
#endif
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_thread.h>

// frontend-common
#include "frend/coprocess.hh"
#include "frend/cursor.hh"
#include "frend/make_cursor.hh"
#include "frend/make_raster.hh"
#include "frend/raster_updating.hh"
#include "frend/update_fifo.hh"

// rasterlib
#include "raster/raster.hh"
#include "raster/relay_detail.hh"
#include "raster/sync.hh"

// v68k-cursor
#include "cursor/cursor.hh"

// Pearl
#include "Pearl/events.hh"


namespace Pearl
{

using raster::raster_desc;
using raster::raster_load;

static bool monitoring;

static SDL_Thread* raster_thread;

static
void raster_event_loop( const raster::sync_relay* sync )
{
	const SDL_EventType eventClass = (SDL_EventType) pearl_event_class;
	const Sint32        repaintDue = kEventPearlUpdate;
	const Sint32        screenBits = kEventPearlScreenBits;
	const Sint32        cursorBits = kEventPearlCursorBits;

	SDL_UserEvent repaint_due = { eventClass };
	SDL_UserEvent screen_bits = { eventClass };
	SDL_UserEvent cursor_bits = { eventClass };
	repaint_due.code = repaintDue;
	screen_bits.code = screenBits;
	cursor_bits.code = cursorBits;

	uint32_t raster_seed = 0;
	uint16_t cursor_seed = 0;

	while ( monitoring  &&  sync->status == raster::Sync_ready )
	{
		using frend::cursor_state;
		using frend::wait_for_update;

		wait_for_update();

		if ( cursor_state  &&  cursor_state->seed != cursor_seed )
		{
			cursor_seed = cursor_state->seed;

			SDL_PushEvent( (SDL_Event*) &cursor_bits );
		}

		if ( raster_seed != sync->seed )
		{
			raster_seed = sync->seed;

			SDL_PushEvent( (SDL_Event*) &screen_bits );
		}

		SDL_PushEvent( (SDL_Event*) &repaint_due );
	}
}

static
int raster_thread_entry( void* arg )
{
	const raster::sync_relay* sync = (const raster::sync_relay*) arg;

	raster_event_loop( sync );

	SDL_QuitEvent quitEvent = { SDL_QUIT };
	return SDL_PushEvent( (SDL_Event*) &quitEvent ) ? 0 : 1;
}

raster_monitor::raster_monitor( const raster::raster_load& load )
{
	pearl_event_class = SDL_RegisterEvents( 1 );

	const raster::sync_relay* sync = find_sync( &load.meta->note );

	monitoring = true;

	raster_thread = SDL_CreateThread( &raster_thread_entry, NULL, (void*) sync );
}

raster_monitor::~raster_monitor()
{
	monitoring = false;

	SDL_WaitThread( raster_thread, NULL );
}

static const char raster_path[] = "screen.skif";
static const char cursor_path[] = "cursor.skif";

emulated_screen::emulated_screen( int bindir_fd, const char* works_path )
:
	live_cursor       ( cursor_path ),
	live_raster       ( raster_path ),
	monitored_raster  ( live_raster.get() ),
	launched_coprocess( bindir_fd, works_path )
{
	events_fd = launched_coprocess.socket();
}

}
