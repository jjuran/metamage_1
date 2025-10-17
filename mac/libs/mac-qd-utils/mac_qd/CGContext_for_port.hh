/*
	CGContext_for_port.hh
	---------------------
*/

#ifndef MACQD_CGCONTEXTFORPORT_HH
#define MACQD_CGCONTEXTFORPORT_HH

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
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// missing-macos
#ifdef MAC_OS_X_VERSION_10_7
#ifndef MISSING_QUICKDRAW_H
#include "missing/Quickdraw.h"
#endif
#endif

// iota
#include "iota/class.hh"


namespace mac {
namespace qd  {

class CGContext_for_port
{
	NON_COPYABLE( CGContext_for_port )
	
	private:
		CGContextRef context;
		CGRect       bounds_rect;
	
	public:
		CGContext_for_port( CGrafPtr port );
		~CGContext_for_port();
		
		operator CGContextRef() const  { return context; }
		
		const CGRect& bounds() const  { return bounds_rect; }
};

class CGContext_for_thePort : public CGContext_for_port
{
	public:
		CGContext_for_thePort() : CGContext_for_port( GetQDGlobalsThePort() )
		{
		}
};

}
}

#endif
