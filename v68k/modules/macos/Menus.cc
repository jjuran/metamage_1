/*
	Menus.cc
	--------
*/

#include "Menus.hh"

// macos
#include "QDGlobals.hh"


short MBarHeight : 0x0BAA;

const short default_MBarHeight = 20;

static
void draw_menu_bar_from_WMgr_port()
{
	const QDGlobals& qd = get_QDGlobals();
	
	Rect menu_bar = qd.screenBits.bounds;
	
	const short bottom = MBarHeight;
	
	menu_bar.bottom = bottom;      EraseRect( &menu_bar );
	menu_bar.top    = bottom - 1;  PaintRect( &menu_bar );
}

pascal void InitMenus_patch()
{
	MBarHeight = default_MBarHeight;
	
	draw_menu_bar_from_WMgr_port();
}

pascal void FlashMenuBar_patch( short menuID )
{
	QDGlobals& qd = get_QDGlobals();
	
	GrafPtr saved_port = qd.thePort;
	
	Rect menu_bar = qd.screenBits.bounds;
	
	menu_bar.bottom = MBarHeight;
	
	InvertRect( &menu_bar );
	
	qd.thePort = saved_port;
}
