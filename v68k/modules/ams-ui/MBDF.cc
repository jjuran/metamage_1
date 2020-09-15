/*
	MBDF.cc
	-------
*/

#include "MBDF.hh"

// ams-common
#include "QDGlobals.hh"
#include "raster_lock.hh"


short MBarHeight : 0x0BAA;


void calculate_menu_bar_height()
{
	const short default_MBarHeight = 20;
	
	if ( MBarHeight < 0 )
	{
		MBarHeight = default_MBarHeight;
	}
}

void draw_menu_bar_from_WMgr_port()
{
	// WMgrPort and a suitable clipRgn have already been set.
	
	const QDGlobals& qd = get_QDGlobals();
	
	Rect menu_bar = qd.screenBits.bounds;
	
	const short bottom = MBarHeight;
	
	raster_lock lock;
	
	menu_bar.bottom = bottom;      EraseRect( &menu_bar );
	menu_bar.top    = bottom - 1;  PaintRect( &menu_bar );
}
