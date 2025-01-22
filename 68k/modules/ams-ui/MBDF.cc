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


short ScreenRow  : 0x0106;
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
	
	const Byte black = 0xFF;
	
	const QDGlobals& qd = get_QDGlobals();
	
	Rect menu_bar = qd.screenBits.bounds;
	
	raster_lock lock;
	
	fast_memset( qd.screenBits.baseAddr, black, ScreenRow * MBarHeight );
	
	menu_bar.bottom = MBarHeight - 1;
	
	EraseRect( &menu_bar );
}
