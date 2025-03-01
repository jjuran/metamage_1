/*
	blitter.hh
	----------
*/

#ifndef PEARL_BLITTER_HH
#define PEARL_BLITTER_HH

// libsdl2
#include <SDL2/SDL_render.h>


namespace Pearl
{

class Blitter
{
	private:
		// non-copyable
		Blitter           ( const Blitter& );
		Blitter& operator=( const Blitter& );

		static bool refresh();
		static bool represent( bool use_integer_scaling );

	public:
		Blitter( SDL_Window* window );
		~Blitter();

		operator SDL_Renderer*() const;

		static bool prep( int stride, int width, int height, int depth );

		static bool blit( const void* src_addr );

		static bool size( int width, int height );

		static bool toggle_integer_scaling();
};

}

#endif
