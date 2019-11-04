/*
	Menus.hh
	--------
*/

#ifndef MENUS_HH
#define MENUS_HH

struct MenuInfo;
struct Point;

typedef pascal void (*SysBeep_ProcPtr)( short duration );

extern SysBeep_ProcPtr old_SysBeep;

pascal void InitMenus_patch();

pascal MenuInfo** NewMenu_patch( short menuID, const unsigned char* title );

pascal MenuInfo** GetMenu_patch( short resID );

pascal void DisposeMenu_patch( MenuInfo** menu );

pascal void AppendMenu_patch( MenuInfo** menu, const unsigned char* format );

pascal void AddResMenu_patch( MenuInfo** menu, unsigned long type );

pascal void InsertMenu_patch( MenuInfo** menu, short beforeID );
pascal void DeleteMenu_patch( short menuID );

pascal void DrawMenuBar_patch();
pascal void ClearMenuBar_patch();

pascal char** GetNewMBar_patch( short menuBarID );

pascal char** GetMenuBar_patch();

pascal void SetMenuBar_patch( char** list );

pascal long MenuSelect_patch( Point start );

pascal long MenuKey_patch( unsigned short key );  // char

pascal void HiliteMenu_patch( short menuID );

pascal void SetItem_patch( MenuInfo** menu, short i, const unsigned char* txt );
pascal void GetItem_patch( MenuInfo** menu, short item, unsigned char* result );

pascal void DisableItem_patch( MenuInfo** menu, short item );
pascal void EnableItem_patch ( MenuInfo** menu, short item );

pascal void CheckItem_patch( MenuInfo** menu, short item, char checked );
pascal void SetItemMark_patch( MenuInfo** menu, short item, char mark );

pascal short CountMItems_patch( MenuInfo** menu );

pascal MenuInfo** GetMHandle_patch( short menuID );

pascal void FlashMenuBar_patch( short menuID );

pascal void InsMenuItem_patch( MenuInfo**            menu,
                               const unsigned char*  s,
                               short                 after );

pascal void SysBeep_patch( short duration );

#endif
