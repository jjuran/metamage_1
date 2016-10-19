/*
	HIViewPlotIconRef.hh
	--------------------
*/

#ifndef PEDESTAL_HIVIEWPLOTICONREF_HH
#define PEDESTAL_HIVIEWPLOTICONREF_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#else
typedef struct CGContext *CGContextRef;
struct CGRect;
typedef struct OpaqueIconRef* IconRef;
#endif


namespace Pedestal
{
	
	void HIViewPlotIconRef( CGContextRef context, CGRect bounds, IconRef icon );
	
}

#endif
