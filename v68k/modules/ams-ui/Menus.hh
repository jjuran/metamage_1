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

pascal void GetItem_patch( MenuInfo** menu, short item, unsigned char* result );
pascal void SetItem_patch( MenuInfo** menu, short i, const unsigned char* txt );

pascal void FlashMenuBar_patch( short menuID );

pascal short CountMItems_patch( MenuInfo** menu );

pascal MenuInfo** GetMenu_patch( short resID );

#endif
