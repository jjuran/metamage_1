/*
	Carbonate/Controls.hh
	---------------------
	
	Joshua Juran
*/

#ifndef CARBONATE_CONTROLS_HH
#define CARBONATE_CONTROLS_HH

#ifndef __CONTROLS__
#include <Controls.h>
#endif

#if TARGET_API_MAC_CARBON
#error Configuration error:  This file is for classic only
#endif

/*
	We need these definitions under the following conditions:
	1. Accessors are not functions, for including
	2. Accessors are functions on 68K (no CarbonAccessors.o), for compiling
	
	CARBONATE_LINKAGE is only defined (to 'pascal') if we're compiling.
*/

#if defined(CARBONATE_LINKAGE) && ACCESSOR_CALLS_ARE_FUNCTIONS && TARGET_CPU_68K || !defined(CARBONATE_LINKAGE) && !ACCESSOR_CALLS_ARE_FUNCTIONS

#ifndef CARBONATE_LINKAGE
#define CARBONATE_LINKAGE inline pascal
#endif

struct popupPrivateData
{
	MenuHandle  mHandle;
	short       mID;
	/* private data follow */
};

CARBONATE_LINKAGE Rect* GetControlBounds( ControlRef control, Rect* bounds )
{
	*bounds = control[0]->contrlRect;
	
	return bounds;
}

CARBONATE_LINKAGE Boolean IsControlHilited( ControlRef control )
{
	return control[0]->contrlHilite != 0;  /* FIXME */
}

CARBONATE_LINKAGE UInt16 GetControlHilite( ControlRef control )
{
	return control[0]->contrlHilite;
}

CARBONATE_LINKAGE WindowRef GetControlOwner( ControlRef control )
{
	return control[0]->contrlOwner;
}

CARBONATE_LINKAGE Handle GetControlDataHandle( ControlRef control )
{
	return control[0]->contrlData;
}

CARBONATE_LINKAGE MenuRef GetControlPopupMenuHandle( ControlRef control )
{
	return ((struct popupPrivateData*) *control[0]->contrlData )->mHandle;  /* reinterpret_cast */
}

CARBONATE_LINKAGE short GetControlPopupMenuID( ControlRef control )
{
	return ((struct popupPrivateData*) *control[0]->contrlData )->mID;  /* reinterpret_cast */
}

CARBONATE_LINKAGE void SetControlDataHandle( ControlRef control, Handle dataHandle )
{
	control[0]->contrlData = dataHandle;
}

CARBONATE_LINKAGE void SetControlBounds( ControlRef control, const Rect* bounds )
{
	control[0]->contrlRect = *bounds;
}

CARBONATE_LINKAGE void SetControlPopupMenuHandle( ControlRef control, MenuRef popupMenu )
{
	((struct popupPrivateData*) *control[0]->contrlData )->mHandle = popupMenu;  /* reinterpret_cast */
}

CARBONATE_LINKAGE void SetControlPopupMenuID( ControlRef control, short menuID )
{
	((struct popupPrivateData*) *control[0]->contrlData )->mID = menuID;  /* reinterpret_cast */
}

#undef CARBONATE_LINKAGE

#endif

#endif

