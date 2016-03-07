/*
	Menus.cc
	--------
*/

#include "Menus.hh"

// macos
#include "MBDF.hh"
#include "QDGlobals.hh"
#include "Windows.hh"


GrafPtr WMgrPort : 0x09DE;
short MBarHeight : 0x0BAA;


pascal void InitMenus_patch()
{
	calculate_menu_bar_height();
	
	draw_menu_bar_from_WMgr_port();
}

pascal void FlashMenuBar_patch( short menuID )
{
	QDGlobals& qd = get_QDGlobals();
	
	GrafPtr saved_port = qd.thePort;
	
	qd.thePort = WMgrPort;
	
	SetClip( BezelRgn );
	
	Rect menu_bar = qd.screenBits.bounds;
	
	menu_bar.bottom = MBarHeight;
	
	InvertRect( &menu_bar );
	
	qd.thePort = saved_port;
}
