/*
	blit_CGL.hh
	-----------
*/

#ifndef BLITCGL_HH
#define BLITCGL_HH

// Mac OS X
#ifdef __APPLE__
// Including just <CoreGraphics/CoreGraphics.h> fails in Mac OS X 10.4.
#include <ApplicationServices/ApplicationServices.h>
#endif


class CGL_blitter
{
	private:
		// non-copyable
		CGL_blitter           ( const CGL_blitter& );
		CGL_blitter& operator=( const CGL_blitter& );
	
	public:
		CGL_blitter( CGDirectDisplayID id, CGRect bounds );
		~CGL_blitter();
		
		static void prep( int stride, int width, int height );
		
		static void blit( const void* src_addr );
};

#endif
