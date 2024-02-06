/*
	blit_QD.hh
	----------
*/

#ifndef BLITQD_HH
#define BLITQD_HH

// Mac OS X
#ifdef __APPLE__
// Including just <CoreGraphics/CoreGraphics.h> fails in Mac OS X 10.4.
#include <ApplicationServices/ApplicationServices.h>
#endif


class QD_blitter
{
	private:
		// non-copyable
		QD_blitter           ( const QD_blitter& );
		QD_blitter& operator=( const QD_blitter& );
	
	public:
		explicit QD_blitter( CGDirectDisplayID id );
		~QD_blitter();
		
		static void prep( int stride, int width, int height, int depth = 1 );
		
		static void area( CGRect bounds );
		
		static void blit( const void* src_addr );
};

#endif
