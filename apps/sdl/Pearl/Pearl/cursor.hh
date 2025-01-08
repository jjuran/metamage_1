/*
	cursor.hh
	---------
*/

#ifndef PEARL_CURSOR_HH
#define PEARL_CURSOR_HH

// frontend-common
#include "frend/cursor.hh"

// libsdl2
#include <SDL2/SDL_rect.h>


struct SDL_Cursor;

namespace Pearl
{

class Blitter;

class Cursor
{
	public:
		enum CompositeMode
		{
			CompositeMode_software = 0,
			CompositeMode_texture  = 1,
			CompositeMode_host_OS  = 2,
		};

	private:
		SDL_Point limit;
		SDL_Point last_location;

		Blitter& blitter;

		CompositeMode preferred_composite_mode;

		SDL_Cursor* host_cursor;

		void reset();

		void pin( SDL_Point& location );

		// non-copyable
		Cursor           ( const Cursor& );
		Cursor& operator=( const Cursor& );

	public:
		Cursor( const SDL_Point& limit, const SDL_Point& initial_location, Blitter& blitter );
		~Cursor();

		void move_to( SDL_Point location );
		SDL_Point get_last_location() const;

		void send_mouse_moved_event( const SDL_Point& location );
		void send_mouse_button_event( uint8_t modes, bool button_down );

		bool show( bool visible );

		bool set( const frend::shared_cursor_state& cursor );

		void set_composite_mode( CompositeMode composite_mode );
		CompositeMode get_composite_mode() const;
};

}

#endif
