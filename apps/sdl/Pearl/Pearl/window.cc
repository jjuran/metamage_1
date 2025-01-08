/*
	window.cc
	---------
*/

#include "Pearl/window.hh"

// Standard C++
#include <algorithm>

// Standard C
#include <math.h>

// libsdl2
#include <SDL2/SDL_video.h>

// Pearl
#include "Pearl/blitter.hh"


namespace Pearl
{

Window::Window( const char* title,
                uint32_t screen_width,
                uint32_t screen_height,
                uint32_t window_width,
                uint32_t window_height ) : screen_w( screen_width ),
                                           screen_h( screen_height )
{
	window = SDL_CreateWindow( title,
	                           SDL_WINDOWPOS_UNDEFINED,
	                           SDL_WINDOWPOS_UNDEFINED,
	                           window_width  ? window_width  : screen_width,
	                           window_height ? window_height : screen_height,
	                           SDL_WINDOW_RESIZABLE );

	if ( window != NULL )
	{
		SDL_SetWindowKeyboardGrab( window, SDL_TRUE );
		SDL_SetWindowMouseGrab( window, SDL_TRUE );
	}
}

Window::~Window()
{
	if ( window != NULL )
	{
		SDL_DestroyWindow( window );
	}
}

Window::operator SDL_Window*() const
{
	return window;
}

void Window::update( const raster_load& load, const raster_desc& desc, Blitter& blitter )
{
	const uint32_t offset = desc.height * desc.stride * desc.frame;

	blitter.blit( (char*) load.addr + offset );
}

bool Window::scale( float scale_multiple )
{
	if ( window == NULL )
	{
		return false;
	}

	int w;
	int h;

	SDL_GetWindowSize( window, &w, &h );

	float this_scale = std::min( (float) w / screen_w, (float) h / screen_h ) / fabs( scale_multiple );
	float base_scale = scale_multiple < 0.0 ? ceilf( this_scale ) : floorf( this_scale );
	float next_scale = ( base_scale + copysignf( 1.0, scale_multiple ) ) / this_scale;

	SDL_SetWindowSize( window,
	                   (int) w * next_scale,
	                   (int) h * next_scale );

	return true;
}

bool Window::toggle_fullscreen()
{
	if ( window == NULL )
	{
		return false;
	}

	bool was_fullscreen = SDL_GetWindowFlags( window ) & SDL_WINDOW_FULLSCREEN;

	if ( was_fullscreen )
	{
		bool windowed = SDL_SetWindowFullscreen( window, 0 ) == 0;

		if ( windowed )
		{
			SDL_SetWindowSize( window, windowed_w, windowed_h );
			SDL_SetWindowPosition( window, windowed_x, windowed_y );
		}

		return windowed;
	}
	else
	{
		SDL_GetWindowSize( window, &windowed_w, &windowed_h );
		SDL_GetWindowPosition( window, &windowed_x, &windowed_y );
		return SDL_SetWindowFullscreen( window, SDL_WINDOW_FULLSCREEN_DESKTOP ) == 0;
	}

	return false;
}

bool Window::toggle_keyboard_grab()
{
	if ( window == NULL )
	{
		return false;
	}

	SDL_bool was_grabbed = SDL_GetWindowKeyboardGrab( window );
	SDL_SetWindowKeyboardGrab( window, SDL_bool( ! was_grabbed ) );

	return true;
}

bool Window::toggle_mouse_grab()
{
	if ( window == NULL )
	{
		return false;
	}

	SDL_bool was_grabbed = SDL_GetWindowMouseGrab( window );
	SDL_SetWindowMouseGrab( window, SDL_bool( ! was_grabbed ) );

	return true;
}

}
