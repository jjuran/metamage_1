/*
	Menus.hh
	--------
*/

#ifndef MENUS_HH
#define MENUS_HH

struct MenuInfo;

pascal void InitMenus_patch();

pascal void InsertMenu_patch( MenuInfo** menu, short beforeID );

pascal void DrawMenuBar_patch();

pascal void FlashMenuBar_patch( short menuID );

pascal MenuInfo** GetMenu_patch( short resID );

#endif
