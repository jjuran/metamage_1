/*	==========
 *	MenuBar.hh
 *	==========
 */

#ifndef PEDESTAL_MENUBAR_HH
#define PEDESTAL_MENUBAR_HH

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __MENUS__
#include <Menus.h>
#endif


namespace Pedestal
{
	
	void AddMenu( MenuRef menu );
	
	void FixUpAboutMenuItem( MenuRef appleMenu );
	
}

#endif
