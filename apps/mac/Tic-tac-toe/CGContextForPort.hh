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


#define CGContextForPort CGContextForPort_

class CGContextForPort
{
	private:
		CGContextRef context;
		
		Rect create();
		
		void erase( const Rect& portRect, float gray );
		
		void transform( int height );
		
		// non-copyable
		CGContextForPort           ( const CGContextForPort& );
		CGContextForPort& operator=( const CGContextForPort& );
	
	public:
		CGContextForPort();
		CGContextForPort( float erase_gray );
		
		~CGContextForPort();
		
		operator CGContextRef() const  { return context; }
};

#endif
