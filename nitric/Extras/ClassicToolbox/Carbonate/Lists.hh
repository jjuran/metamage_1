// Carbonate/Lists.hh

#ifndef CARBONATE_LISTS_HH
#define CARBONATE_LISTS_HH

#ifndef __LISTS__
#include <Lists.h>
#endif

#if TARGET_API_MAC_CARBON
#error Configuration error:  This file is for classic only
#endif

#if TARGET_CPU_68K && TARGET_RT_MAC_CFM

#endif


// We need these definitions under the following conditions:
// 1. Accessors are functions on 68K (no CarbonAccessors.o), for compiling
// 2. Accessors are not functions, for including

// CARBONATE_LINKAGE is only defined (to 'pascal') if we're compiling.

#if defined(CARBONATE_LINKAGE) && ACCESSOR_CALLS_ARE_FUNCTIONS && TARGET_CPU_68K || !defined(CARBONATE_LINKAGE) && !ACCESSOR_CALLS_ARE_FUNCTIONS

#ifndef CARBONATE_LINKAGE
#define CARBONATE_LINKAGE inline pascal
#endif

CARBONATE_LINKAGE Rect* GetListViewBounds( ListHandle list, Rect* view )
{
	*view = list[0]->rView;
	
	return view;
}

CARBONATE_LINKAGE ListBounds* GetListVisibleCells( ListHandle list, ListBounds* visible )
{
	*visible = list[0]->visible;
	
	return visible;
}

CARBONATE_LINKAGE ControlRef GetListVerticalScrollBar( ListHandle list )
{
	return list[0]->vScroll;
}

CARBONATE_LINKAGE ControlRef GetListHorizontalScrollBar( ListHandle list )
{
	return list[0]->hScroll;
}

CARBONATE_LINKAGE ListBounds* GetListDataBounds( ListHandle list, ListBounds* bounds )
{
	*bounds = list[0]->dataBounds;
	
	return bounds;
}

CARBONATE_LINKAGE OptionBits GetListFlags( ListHandle list )
{
	return list[0]->listFlags;
}

CARBONATE_LINKAGE void SetListViewBounds( ListHandle list, const Rect* view )
{
	list[0]->rView = *view;
}

#undef CARBONATE_LINKAGE

#endif

#endif

