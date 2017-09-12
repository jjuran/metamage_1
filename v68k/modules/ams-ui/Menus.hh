/*
	Menus.hh
	--------
*/

#ifndef MENUS_HH
#define MENUS_HH

struct MenuInfo;
struct Point;

pascal void InitMenus_patch();

pascal void InsertMenu_patch( MenuInfo** menu, short beforeID );

pascal void DrawMenuBar_patch();

pascal void HiliteMenu_patch( short menuID );

pascal long MenuSelect_patch( Point start );

pascal void GetItem_patch( MenuInfo** menu, short item, unsigned char* result );
pascal void SetItem_patch( MenuInfo** menu, short i, const unsigned char* txt );

pascal void FlashMenuBar_patch( short menuID );

pascal void AddResMenu_patch( MenuInfo** menu, unsigned long type );

pascal short CountMItems_patch( MenuInfo** menu );

pascal MenuInfo** GetMenu_patch( short resID );

#endif
