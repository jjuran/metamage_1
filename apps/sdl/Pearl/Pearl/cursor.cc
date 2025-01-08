/*
	cursor.cc
	---------
*/

#include "Pearl/cursor.hh"

// libsdl2
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>

// splode
#include "splode/splode.hh"

// v68k-cursor
#include "cursor/cursor.hh"

// write-a-splode
#include "splode/write-a-splode.hh"

// frontend-common
#include "frend/pinned.hh"

// Pearl
#include "Pearl/blitter.hh"
#include "Pearl/events.hh"


namespace Pearl
{

Cursor::Cursor( const SDL_Point& limit,
                const SDL_Point& initial_location,
                Blitter& blitter ) : limit( limit ),
                                     last_location( initial_location ),
                                     blitter( blitter ),
                                     preferred_composite_mode( CompositeMode_software ),
                                     host_cursor( NULL )
{
	move_to( initial_location );

	// Don't show double cursors while the window has focus.
	SDL_ShowCursor( SDL_FALSE );
}

Cursor::~Cursor()
{
	reset();
}

void Cursor::reset()
{
	if ( host_cursor != NULL )
	{
		SDL_FreeCursor( host_cursor );
		host_cursor = NULL;
	}
}

template < class T >
static inline
bool is_point_equal_to_point( T a, T b )
{
	return a.x == b.x  &&  a.y == b.y;
}

void Cursor::pin( SDL_Point& location )
{
	SDL_Point unpinned_location = location;

	location = frend::pinned_location( unpinned_location, limit );

	if ( SDL_GetWindowMouseGrab( SDL_RenderGetWindow( blitter ) )  &&
	     ! is_point_equal_to_point( location, unpinned_location ) )
	{
		move_to( location );
	}
}

void Cursor::move_to( SDL_Point location )
{
	SDL_Renderer* renderer = (SDL_Renderer*) blitter;

	if ( renderer != NULL )
	{
		SDL_RenderLogicalToWindow( renderer,
		                           (float) location.x,
		                           (float) location.y,
		                           &location.x,
		                           &location.y );
	}

	SDL_WarpMouseInWindow( SDL_RenderGetWindow( blitter ), location.x, location.y );
}

SDL_Point Cursor::get_last_location() const
{
	return last_location;
}

void Cursor::send_mouse_moved_event( const SDL_Point& location )
{
	SDL_Point pinned_location = location;

	pin( pinned_location );

	if ( ! is_point_equal_to_point( pinned_location, last_location ) )
	{
		using splode::send_mouse_moved_event;

		send_mouse_moved_event( events_fd, pinned_location.x, pinned_location.y );

		last_location = pinned_location;
	}
}

void Cursor::send_mouse_button_event( uint8_t modes, bool button_down )
{
	using namespace splode::pointer;

	uint8_t attrs = 0;
	if ( button_down )
	{
		attrs |= down;
	}
	else
	{
		attrs |= up;
	}

	splode::send_mouse_event( events_fd, modes, attrs );
}

bool Cursor::show( bool visible )
{
	return SDL_ShowCursor( visible ) >= 0;
}

bool Cursor::set( const frend::shared_cursor_state& cursor )
{
	if ( preferred_composite_mode != CompositeMode_host_OS )
	{
		// TODO
		return false;
	}

	reset();

	host_cursor = SDL_CreateCursor( (uint8_t*) cursor.face,
	                                (uint8_t*) cursor.mask,
	                                16, 16,
	                                cursor.hotspot[ 1 ],
	                                cursor.hotspot[ 0 ] );

	if ( host_cursor == NULL )
	{            
		return false;
	}

	SDL_SetCursor( host_cursor );	

	return true;
}

void Cursor::set_composite_mode( CompositeMode mode )
{
	if ( preferred_composite_mode == mode )
	{
		return;
	}

	switch ( preferred_composite_mode )
	{
		case CompositeMode_texture:
			// TODO
			break;

		case CompositeMode_host_OS:
			reset();
			break;

		default:
			break;
	}

	preferred_composite_mode = mode;
}

Cursor::CompositeMode Cursor::get_composite_mode() const
{
	return preferred_composite_mode;
}

}
