/*
	Menus.cc
	--------
*/

#include "Menus.hh"

// macos
#include "QDGlobals.hh"


short MBarHeight : 0x0BAA;

const short default_MBarHeight = 20;

pascal void InitMenus_patch()
{
	MBarHeight = default_MBarHeight;
}
