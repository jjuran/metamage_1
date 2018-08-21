/*
	Menus.hh
	--------
*/

#ifndef MENUS_HH
#define MENUS_HH

struct MenuInfo;
struct Point;

typedef unsigned char Boolean;
typedef unsigned long ResType;

typedef short CharParameter;
typedef short StyleParameter;

typedef unsigned char const* ConstStr255Param;
typedef unsigned char*       StringPtr;

typedef char** Handle;

typedef MenuInfo** MenuRef;

typedef pascal void (*SysBeep_ProcPtr)( short duration );

extern SysBeep_ProcPtr old_SysBeep;

pascal void InitMenus_patch();

pascal MenuRef NewMenu_patch( short menuID, ConstStr255Param title );

pascal MenuRef GetMenu_patch( short resID );

pascal void DisposeMenu_patch( MenuRef menu );

pascal void AppendMenu_patch( MenuRef menu, ConstStr255Param format );

pascal void AddResMenu_patch( MenuRef menu, ResType type );

pascal short OpenDeskAcc_patch( ConstStr255Param name );

pascal void InsertMenu_patch( MenuRef menu, short beforeID );
pascal void DeleteMenu_patch( short menuID );

pascal void DrawMenuBar_patch();
pascal void ClearMenuBar_patch();

pascal Handle GetNewMBar_patch( short menuBarID );

pascal Handle GetMenuBar_patch();

pascal void SetMenuBar_patch( Handle list );

pascal long MenuSelect_patch( Point start );

pascal long MenuKey_patch( CharParameter key );

pascal void HiliteMenu_patch( short menuID );

pascal void SetItem_patch( MenuRef menu, short i, ConstStr255Param txt );
pascal void GetItem_patch( MenuRef menu, short item, StringPtr result );

pascal void DisableItem_patch( MenuRef menu, short item );
pascal void EnableItem_patch ( MenuRef menu, short item );

pascal void CheckItem_patch( MenuRef menu, short item, Boolean checked );
pascal void SetItemMark_patch( MenuRef menu, short item, CharParameter  mark );
pascal void GetItemMark_patch( MenuRef menu, short item, CharParameter* mark );

pascal void SetItmIcon_patch( MenuRef menu, short item, short icon );

pascal void SetItemStyle_patch( MenuRef menu, short i, StyleParameter style );

pascal short CountMItems_patch( MenuRef menu );

pascal MenuRef GetMHandle_patch( short menuID );

pascal void FlashMenuBar_patch( short menuID );

pascal void InsMenuItem_patch( MenuRef           menu,
                               ConstStr255Param  s,
                               short             after );

pascal void DelMenuItem_patch( MenuRef menu, short item );

pascal void GetItemCmd_patch( MenuRef menu, short item, CharParameter* key );
pascal void SetItemCmd_patch( MenuRef menu, short item, CharParameter  key );

pascal void SysBeep_patch( short duration );

#endif
