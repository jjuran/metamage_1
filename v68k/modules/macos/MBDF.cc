/*
	MBDF.cc
	-------
*/

#include "MBDF.hh"

// macos
#include "QDGlobals.hh"
#include "screen_lock.hh"


short MBarHeight : 0x0BAA;


void calculate_menu_bar_height()
{
	const short default_MBarHeight = 20;
	
	MBarHeight = default_MBarHeight;
}

void draw_menu_bar_from_WMgr_port()
{
	// WMgrPort and a suitable clipRgn have already been set.
	
	const QDGlobals& qd = get_QDGlobals();
	
	Rect menu_bar = qd.screenBits.bounds;
	
	const short bottom = MBarHeight;
	
	screen_lock lock;
	
	menu_bar.bottom = bottom;      EraseRect( &menu_bar );
	menu_bar.top    = bottom - 1;  PaintRect( &menu_bar );
}
