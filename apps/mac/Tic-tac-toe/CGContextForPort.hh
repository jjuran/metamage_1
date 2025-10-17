/*
	CGContextForPort.hh
	-------------------
*/

#ifndef TICTACTOE_CGCONTEXTFORPORT_HH
#define TICTACTOE_CGCONTEXTFORPORT_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef CGCONTEXT_H_
#ifndef __CGCONTEXT__
#include <CGContext.h>
#endif
#endif

// iota
#include "iota/class.hh"


#define CGContextForPort CGContextForPort_

class CGContextForPort
{
	NON_COPYABLE( CGContextForPort )
	
	private:
		CGContextRef context;
		CGRect       bounds_rect;
		
		Rect create();
		
		void erase( const Rect& portRect, float gray );
		
		void transform( int height );
	
	public:
		CGContextForPort();
		CGContextForPort( float erase_gray );
		
		~CGContextForPort();
		
		operator CGContextRef() const  { return context; }
		
		const CGRect& bounds() const  { return bounds_rect; }
};

#endif
