// CarbonUnits/Controls.hh

#ifndef CARBONUNITS_CONTROLS_HH
#define CARBONUNITS_CONTROLS_HH

#ifndef __CONTROLS__
#include <Controls.h>
#endif

#if TARGET_API_MAC_CARBON
#error Configuration error:  This file is for classic only
#endif

// We need these definitions under the following conditions:
// 1. Accessors are functions on 68K (no CarbonAccessors.o), for compiling
// 2. Accessors are not functions, for including

// CARBONUNITS_LINKAGE is only defined (to 'pascal') if we're compiling.

#if defined(CARBONUNITS_LINKAGE) && ACCESSOR_CALLS_ARE_FUNCTIONS && TARGET_CPU_68K || !defined(CARBONUNITS_LINKAGE) && !ACCESSOR_CALLS_ARE_FUNCTIONS

#ifndef CARBONUNITS_LINKAGE
#define CARBONUNITS_LINKAGE inline pascal
#endif

struct popupPrivateData
{
	MenuHandle  mHandle;
	short       mID;
	// private data follow
};

CARBONUNITS_LINKAGE Rect* GetControlBounds( ControlRef control, Rect* bounds )
{
	*bounds = control[0]->contrlRect;
	
	return bounds;
}

CARBONUNITS_LINKAGE Boolean IsControlHilited( ControlRef control )
{
	return control[0]->contrlHilite != 0;  // FIXME
}

CARBONUNITS_LINKAGE UInt16 GetControlHilite( ControlRef control )
{
	return control[0]->contrlHilite;
}

CARBONUNITS_LINKAGE WindowRef GetControlOwner( ControlRef control )
{
	return control[0]->contrlOwner;
}

CARBONUNITS_LINKAGE Handle GetControlDataHandle( ControlRef control )
{
	return control[0]->contrlData;
}

CARBONUNITS_LINKAGE MenuRef GetControlPopupMenuHandle( ControlRef control )
{
	return reinterpret_cast< popupPrivateData* >( *control[0]->contrlData )->mHandle;
}

CARBONUNITS_LINKAGE short GetControlPopupMenuID( ControlRef control )
{
	return reinterpret_cast< popupPrivateData* >( *control[0]->contrlData )->mID;
}

CARBONUNITS_LINKAGE void SetControlDataHandle( ControlRef control, Handle dataHandle )
{
	control[0]->contrlData = dataHandle;
}

CARBONUNITS_LINKAGE void SetControlBounds( ControlRef control, const Rect* bounds )
{
	control[0]->contrlRect = *bounds;
}

CARBONUNITS_LINKAGE void SetControlPopupMenuHandle( ControlRef control, MenuRef popupMenu )
{
	reinterpret_cast< popupPrivateData* >( *control[0]->contrlData )->mHandle = popupMenu;
}

CARBONUNITS_LINKAGE void SetControlPopupMenuID( ControlRef control, short menuID )
{
	reinterpret_cast< popupPrivateData* >( *control[0]->contrlData )->mID = menuID;
}

#undef CARBONUNITS_LINKAGE

#endif

#endif

