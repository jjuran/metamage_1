/*
	window.hh
	---------
*/

#ifndef PEARL_WINDOW_HH
#define PEARL_WINDOW_HH

// frontend-common
#include "frend/make_raster.hh"


struct SDL_Window;

namespace Pearl
{

class Blitter;
class Cursor;

using raster::raster_load;
using raster::raster_desc;

class Window
{
	private:
		SDL_Window* window;

		uint32_t screen_w;
		uint32_t screen_h;

		int windowed_x;
		int windowed_y;
		int windowed_w;
		int windowed_h;

		// non-copyable
		Window           ( const Window& );
		Window& operator=( const Window& );

	public:
		Window( const char* title, uint32_t screen_width, uint32_t screen_height, uint32_t window_width = 0, uint32_t window_height = 0 );
		~Window();

		operator SDL_Window*() const;

		void update( const raster_load& load, const raster_desc& desc, Blitter& blitter );

		bool scale( float scale_factor );

		bool toggle_fullscreen();
		bool toggle_keyboard_grab();
		bool toggle_mouse_grab();
};

}

#endif
