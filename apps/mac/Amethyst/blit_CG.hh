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


class CG_blitter
{
	private:
		// non-copyable
		CG_blitter           ( const CG_blitter& );
		CG_blitter& operator=( const CG_blitter& );
	
	public:
		explicit CG_blitter( CGDirectDisplayID id );
		~CG_blitter();
		
		static void prep( int stride, int width, int height );
		
		static void area( CGRect bounds );
		
		static void blit( const void* src_addr );
};

#endif
