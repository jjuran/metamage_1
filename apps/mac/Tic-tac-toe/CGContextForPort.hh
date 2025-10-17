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


class CGContextForPort
{
	NON_COPYABLE( CGContextForPort )
	
	private:
		CGContextRef context;
		CGRect       bounds_rect;
	
	public:
		CGContextForPort();
		~CGContextForPort();
		
		operator CGContextRef() const  { return context; }
		
		const CGRect& bounds() const  { return bounds_rect; }
};

#endif
