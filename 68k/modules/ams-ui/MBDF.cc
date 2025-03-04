/*
	MBDF.cc
	-------
*/

#include "MBDF.hh"

// ams-common
#include "callouts.hh"
#include "QDGlobals.hh"
#include "raster_lock.hh"


#pragma exceptions off


short CrsrRow    : 0x08AC;
short DepthLog2  : 0x08AE;
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
	
	const Byte black = (DepthLog2 > 3) - 1;  // $00 for RGB, $FF for gray/CLUT
	
	const QDGlobals& qd = get_QDGlobals();
	
	Rect menu_bar = qd.screenBits.bounds;
	
	raster_lock lock;
	
	fast_memset( qd.screenBits.baseAddr, black, CrsrRow * MBarHeight );
	
	menu_bar.bottom = MBarHeight - 1;
	
	EraseRect( &menu_bar );
}
