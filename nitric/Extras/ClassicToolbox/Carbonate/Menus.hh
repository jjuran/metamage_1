// Carbonate/Menus.hh

#ifndef CARBONATE_MENUS_HH
#define CARBONATE_MENUS_HH

#ifndef __MENUS__
#include <Menus.h>
#endif

// Standard C++
#include <algorithm>

#if TARGET_API_MAC_CARBON
#error Configuration error:  This file is for classic only
#endif

#if TARGET_CPU_68K && TARGET_RT_MAC_CFM

inline pascal UInt16 CountMenuItems( MenuRef menu )
{
	return CountMItems( menu );
}

inline pascal void SetMenuFlashCount( short count )
{
	return SetMenuFlash( count );
}

inline pascal void MacCheckMenuItem( MenuRef menu, short item, Boolean checked )
{
	return CheckItem( menu, item, checked );
}

#endif


// We need these definitions under the following conditions:
// 1. Accessors are functions on 68K (no CarbonAccessors.o), for compiling
// 2. Accessors are not functions, for including

// CARBONATE_LINKAGE is only defined (to 'pascal') if we're compiling.

#if defined(CARBONATE_LINKAGE) && ACCESSOR_CALLS_ARE_FUNCTIONS && TARGET_CPU_68K || !defined(CARBONATE_LINKAGE) && !ACCESSOR_CALLS_ARE_FUNCTIONS

#ifndef CARBONATE_LINKAGE
#define CARBONATE_LINKAGE inline pascal
#endif

CARBONATE_LINKAGE MenuID GetMenuID( MenuRef menu )
{
	return menu[0]->menuID;
}

CARBONATE_LINKAGE SInt16 GetMenuWidth( MenuRef menu )
{
	return menu[0]->menuWidth;
}

CARBONATE_LINKAGE SInt16 GetMenuHeight( MenuRef menu )
{
	return menu[0]->menuHeight;
}

CARBONATE_LINKAGE StringPtr GetMenuTitle( MenuRef menu, Str255 title )
{
	return menu[0]->menuData;
}

CARBONATE_LINKAGE void SetMenuID( MenuRef menu, MenuID menuID )
{
	menu[0]->menuID = menuID;
}

CARBONATE_LINKAGE void SetMenuWidth( MenuRef menu, SInt16 width )
{
	menu[0]->menuWidth = width;
}

CARBONATE_LINKAGE void SetMenuHeight( MenuRef menu, SInt16 height )
{
	menu[0]->menuHeight = height;
}

CARBONATE_LINKAGE void SetMenuTitle( MenuRef menu, ConstStr255Param title )
{
	std::copy( title,
	           title + 1 + title[0],
	           menu[0]->menuData );
}

#undef CARBONATE_LINKAGE

#endif

#endif

