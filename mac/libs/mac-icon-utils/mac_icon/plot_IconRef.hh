/*
	plot_IconRef.hh
	---------------
*/

#ifndef MACICON_PLOTICONREF_HH
#define MACICON_PLOTICONREF_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#else
typedef struct CGContext *CGContextRef;
struct CGRect;
typedef struct OpaqueIconRef* IconRef;
#endif


namespace mac  {
namespace icon {
	
	void plot_IconRef( CGContextRef context, CGRect bounds, IconRef icon );
	
}
}

#endif
