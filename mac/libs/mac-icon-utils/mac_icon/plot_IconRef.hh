/*
	plot_IconRef.hh
	---------------
*/

#ifndef MACICON_PLOTICONREF_HH
#define MACICON_PLOTICONREF_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __ICONS__
#include <Icons.h>
#endif


namespace mac  {
namespace icon {
	
#if ! __LP64__
	
	inline
	void plot_IconRef( const Rect& bounds, IconRef icon )
	{
		PlotIconRef( &bounds, 0, 0, kIconServicesNormalUsageFlag, icon );
	}
	
#endif
	
	void plot_IconRef( CGContextRef context, CGRect bounds, IconRef icon );
	
}
}

#endif
