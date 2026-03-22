/*
	blit_CG.hh
	----------
*/

#ifndef BLITCG_HH
#define BLITCG_HH

// Mac OS X
#ifdef __APPLE__
// Including just <CoreGraphics/CoreGraphics.h> fails in Mac OS X 10.4.
#include <ApplicationServices/ApplicationServices.h>
#endif

// iota
#include "iota/class.hh"


class CG_blitter
{
	NON_COPYABLE( CG_blitter )
	
	public:
		explicit CG_blitter( CGDirectDisplayID id );
		~CG_blitter();
		
		static void set_palette( const unsigned short* colors, int n );
		
		static void prep( int stride, int width, int height, int depth );
		
		static void area( CGRect bounds );
		
		static void blit( const void* src_addr );
		
		static void render()  {}
};

#endif
