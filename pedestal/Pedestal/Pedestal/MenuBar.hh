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

// Pedestal
#ifndef PEDESTAL_COMMANDCODE_HH
#include "Pedestal/CommandCode.hh"
#endif


namespace Pedestal
{
	
	void AddMenu( MenuRef menu );
	
	void FixUpAboutMenuItem( MenuRef appleMenu );
	
	CommandCode HandleMenuItem( MenuID menuID, SInt16 item );
	
}

#endif
