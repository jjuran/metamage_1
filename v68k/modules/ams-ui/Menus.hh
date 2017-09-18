/*
	Menus.hh
	--------
*/

#ifndef MENUS_HH
#define MENUS_HH

struct MenuInfo;
struct Point;

pascal void InitMenus_patch();

pascal MenuInfo** GetMenu_patch( short resID );

pascal void AppendMenu_patch( MenuInfo** menu, const unsigned char* format );

pascal void AddResMenu_patch( MenuInfo** menu, unsigned long type );

pascal void InsertMenu_patch( MenuInfo** menu, short beforeID );

pascal void DrawMenuBar_patch();

pascal void HiliteMenu_patch( short menuID );

pascal long MenuSelect_patch( Point start );

pascal void SetItem_patch( MenuInfo** menu, short i, const unsigned char* txt );
pascal void GetItem_patch( MenuInfo** menu, short item, unsigned char* result );

pascal void CheckItem_patch( MenuInfo** menu, short item, char checked );

pascal void FlashMenuBar_patch( short menuID );

pascal short CountMItems_patch( MenuInfo** menu );

#endif
