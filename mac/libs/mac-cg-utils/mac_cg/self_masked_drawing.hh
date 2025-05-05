/*
	self_masked_drawing.hh
	----------------------
*/

#ifndef MACCG_SELFMASKEDDRAWING_HH
#define MACCG_SELFMASKEDDRAWING_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif


namespace mac {
namespace cg  {
	
	/*
		This is a scope-oriented class for compositing QuickDraw text
		into a Core Graphics context without clobbering the background.
		
		For example:
		
			self_masked_drawing drawing( context, bounds );
			
			if ( drawing.port() )
			{
				TETextBox( text, length, drawing.rect(), teJustCenter );
			}
		
		The constructor creates a 32-bit GWorld, sets it as the current
		graphics port, and erases it, after which the caller should
		perform any QuickDraw operations desired (whether text or not).
		
		The destructor restores the previous graphics port, creates an
		ARGB CGImage (interpreting white pixels as transparent), draws
		the image in the context, and releases the image and the GWorld.
	*/
	
	class self_masked_drawing
	{
		private:
			CGContextRef its_context;
			CGRect       its_CGRect;
			Rect         its_QDRect;
			GrafPtr      its_saved_port;
			CGrafPtr     its_gworld;
			
			// non-copyable
			self_masked_drawing           ( const self_masked_drawing& );
			self_masked_drawing& operator=( const self_masked_drawing& );
		
		public:
			self_masked_drawing( CGContextRef context, const CGRect& bounds );
			
			~self_masked_drawing();
			
			bool works() const  { return its_gworld; }
			
			const Rect* rect() const  { return &its_QDRect; }
	};
	
}
}

#endif
