/*
	blitter.cc
	----------
*/

#include "Pearl/blitter.hh"

// libsdl2
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>


namespace Pearl
{

static const SDL_PixelFormatEnum texture_format = SDL_PIXELFORMAT_RGBA8888;

static SDL_Renderer* renderer = NULL;
static SDL_Surface* src_surface = NULL;
static SDL_Palette* src_palette = NULL;
static SDL_Surface* dst_surface = NULL;
static SDL_Texture* texture = NULL;

static int w, h, row_bytes;
static SDL_PixelFormatEnum pix_fmt;

static SDL_ScaleMode scale_mode = SDL_ScaleModeNearest;

static bool prefer_integer_scaling = false;
static bool allow_preferred_modes  = false;

static void reset()
{
	if ( src_surface != NULL )
	{
		SDL_FreeSurface( src_surface );
		src_surface = NULL;
	}

	if ( src_palette != NULL )
	{
		SDL_FreePalette( src_palette );
		src_palette = NULL;
	}

	if ( dst_surface != NULL )
	{
		SDL_FreeSurface( dst_surface );
		dst_surface = NULL;
	}

	if ( texture != NULL )
	{
		SDL_DestroyTexture( texture );
		texture = NULL;
	}
}

Blitter::Blitter( SDL_Window* window )
{
	if ( window != NULL )
	{
		renderer = SDL_CreateRenderer( window, -1, 0 );
	}
}

Blitter::~Blitter()
{
	reset();

	if ( renderer != NULL )
	{
		SDL_RenderClear( renderer );

		SDL_DestroyRenderer( renderer );
		renderer = NULL;
	}
}

Blitter::operator SDL_Renderer*() const
{
	return renderer;
}

bool Blitter::prep( int stride, int width, int height, int depth )
{
	if ( renderer == NULL )
	{
		return false;
	}

	reset();

	w         = width;
	h         = height;
	row_bytes = stride;

	switch ( depth )
	{
		case 1:
			// only monochrome is supported for now
			pix_fmt = SDL_PIXELFORMAT_INDEX1MSB;
			break;

		default:
			return false;
	}

	src_surface = SDL_CreateRGBSurfaceWithFormatFrom( NULL, w, h, 1, row_bytes, pix_fmt );

	if ( src_surface == NULL )
	{
		return false;
	}

	src_palette = SDL_AllocPalette( 2 );

	SDL_Color colors[ 2 ] =
	{
		{ 0xFF, 0xFF, 0xFF, 0xFF },
		{ 0x00, 0x00, 0x00, 0xFF }
	};

	if ( SDL_SetPaletteColors( src_palette, colors, 0, 2 ) != 0  ||
	     SDL_SetSurfacePalette( src_surface, src_palette ) != 0 )
	{
		return false;
	}

	dst_surface = SDL_CreateRGBSurfaceWithFormatFrom( NULL, w, h, 32, w * (32 / 8), texture_format );

	if ( dst_surface == NULL )
	{
		return false;
	}

	texture = SDL_CreateTexture( renderer, texture_format, SDL_TEXTUREACCESS_STREAMING, w, h );

	return SDL_RenderSetLogicalSize( renderer, width, height ) == 0  &&  size( width, height );
}

bool Blitter::blit( const void* src_addr )
{
	if ( renderer == NULL  ||  texture == NULL  ||  src_surface == NULL  ||  dst_surface == NULL )
	{
		return false;
	}

	if ( SDL_LockTexture( texture, NULL, &dst_surface->pixels, &dst_surface->pitch ) == 0 )
	{
		src_surface->pixels = (void*) src_addr;
		src_surface->pitch  = row_bytes;

		SDL_Rect rect =
		{
			0, 0,
			w, h
		};

		bool did_blit = SDL_LowerBlit( src_surface, &rect, dst_surface, &rect ) == 0;

		SDL_UnlockTexture( texture );

		if ( did_blit )
		{
			return refresh();
		}
	}

	return false;
}

bool Blitter::refresh()
{
	bool did_refresh = SDL_RenderClear( renderer ) == 0  &&
	                   SDL_RenderCopy( renderer, texture, NULL, NULL ) == 0;

	if ( ! did_refresh )
	{
		return false;
	}

	SDL_RenderPresent( renderer );

	return did_refresh;
}

bool Blitter::represent( bool use_integer_scaling )
{
	bool did_represent = SDL_SetTextureScaleMode( texture, use_integer_scaling ? SDL_ScaleModeNearest : scale_mode ) == 0  &&
	                     SDL_RenderSetIntegerScale( renderer, SDL_bool( use_integer_scaling ) ) == 0  &&
	                     SDL_RenderClear( renderer ) == 0;

	if ( ! did_represent )
	{
		return false;
	}

	SDL_RenderPresent( renderer );

	return refresh();
}

bool Blitter::size( int width, int height )
{
	if ( renderer == NULL  ||  texture == NULL )
	{
		return false;
	}

	// Preserve aspect ratio when using nearest.
	if ( (  width % w == 0  &&  height >= h *  width / w ) ||
	     ( height % h == 0  &&   width >= w * height / h ) )
	{
		scale_mode = SDL_ScaleModeNearest;
	}
	else
	{
		scale_mode = SDL_ScaleModeLinear;
	}

	if ( width <= w  ||  height <= h )
	{
		allow_preferred_modes = false;
		return represent( false );
	}
	else
	{
		allow_preferred_modes = true;
		return represent( prefer_integer_scaling );
	}
}

bool Blitter::toggle_integer_scaling()
{
	if ( renderer == NULL  ||  texture == NULL  ||  ! allow_preferred_modes )
	{
		return false;
	}

	prefer_integer_scaling = ! prefer_integer_scaling;

	return represent( prefer_integer_scaling );
}

}
