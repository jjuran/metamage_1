/*
	Menus.cc
	--------
*/

#include "Menus.hh"

// ams-common
#include "QDGlobals.hh"

// ams-core
#include "MBDF.hh"
#include "Windows.hh"


GrafPtr WMgrPort : 0x09DE;
short MBarHeight : 0x0BAA;


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
}

pascal void DrawMenuBar_patch()
{
	WMgrPort_bezel_scope port_swap;
	
	draw_menu_bar_from_WMgr_port();
}

pascal void FlashMenuBar_patch( short menuID )
{
	WMgrPort_bezel_scope port_swap;
	
	Rect menu_bar = BezelRgn[0]->rgnBBox;
	
	menu_bar.bottom = MBarHeight;
	
	InvertRect( &menu_bar );
}
