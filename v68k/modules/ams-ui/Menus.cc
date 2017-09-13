/*
	Menus.cc
	--------
*/

#include "Menus.hh"

// Mac OS
#ifndef __MENUS__
#include <Menus.h>
#endif
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// ams-common
#include "QDGlobals.hh"

// ams-core
#include "MBDF.hh"
#include "MDEF.hh"
#include "Windows.hh"


GrafPtr WMgrPort : 0x09DE;
short MBarHeight : 0x0BAA;


struct MenuList_header
{
	UInt16 extent_bytes;
	SInt16 right_edge;
	SInt16 unused;
};

struct MenuList_entry
{
	MenuRef  menu;
	SInt16   left_edge;
};

static short TheMenu;

static MenuList_header** MenuList;

const short system_font_ascent =  9;
const short menu_title_padding =  9;
const short gap_between_titles = 13;  // space between text of adjacent titles


class WMgrPort_bezel_scope
{
	private:
		GrafPtr oldPort;
		
		// non-copyable
		WMgrPort_bezel_scope           ( WMgrPort_bezel_scope& );
		WMgrPort_bezel_scope& operator=( WMgrPort_bezel_scope& );
	
	public:
		WMgrPort_bezel_scope();
		~WMgrPort_bezel_scope();
};

inline
WMgrPort_bezel_scope::WMgrPort_bezel_scope()
{
	GrafPtr& thePort = *get_addrof_thePort();
	
	oldPort = thePort;
	thePort = WMgrPort;
	
	SetClip( BezelRgn );
}

inline
WMgrPort_bezel_scope::~WMgrPort_bezel_scope()
{
	*get_addrof_thePort() = oldPort;
}


pascal void InitMenus_patch()
{
	calculate_menu_bar_height();
	
	draw_menu_bar_from_WMgr_port();
	
	MenuList = (MenuList_header**) NewHandleClear( sizeof (MenuList_header) );
	
	MenuList[0]->right_edge = 10;
}

pascal void InsertMenu_patch( MenuInfo** menu, short beforeID )
{
	// TODO:  Respect beforeID
	
	const UInt16 extent_bytes = MenuList[0]->extent_bytes;
	
	const Size new_size = sizeof (MenuList_header)
	                    + extent_bytes
	                    + sizeof (MenuList_entry);
	
	SetHandleSize( (Handle) MenuList, new_size );
	
	MenuList_header* header = *MenuList;
	
	MenuList_entry* last = (MenuList_entry*) ((char*) *MenuList + extent_bytes);
	MenuList_entry* next = last + 1;
	
	next->menu = menu;
	next->left_edge = header->right_edge;
	
	header->right_edge += gap_between_titles + StringWidth( menu[0]->menuData );
	
	header->extent_bytes += sizeof (MenuList_entry);
	
	MDEF_0( mSizeMsg, menu, NULL, Point(), NULL );
}

pascal void DrawMenuBar_patch()
{
	WMgrPort_bezel_scope port_swap;
	
	draw_menu_bar_from_WMgr_port();
	
	const short pen_v = (MBarHeight + system_font_ascent) / 2;
	
	const UInt16 extent_bytes = MenuList[0]->extent_bytes;
	
	MenuList_entry* last = (MenuList_entry*) ((char*) *MenuList + extent_bytes);
	MenuList_entry* next = (MenuList_entry*) *MenuList;
	
	while ( ++next <= last )
	{
		MenuRef menu = next->menu;
		
		MoveTo( next->left_edge - 1 + menu_title_padding, pen_v );
		
		DrawString( menu[0]->menuData );
	}
}

pascal void FlashMenuBar_patch( short menuID )
{
	WMgrPort_bezel_scope port_swap;
	
	Rect menu_bar = BezelRgn[0]->rgnBBox;
	
	menu_bar.bottom = MBarHeight;
	
	InvertRect( &menu_bar );
}

pascal MenuInfo** GetMenu_patch( short resID )
{
	Handle h = GetResource( 'MENU', resID );
	
	return (MenuRef) h;
}
