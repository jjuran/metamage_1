/*	=====================
 *	ApplicationContext.cc
 *	=====================
 */

#include "Pedestal/ApplicationContext.hh"

// Mac OS
#include <Dialogs.h>


namespace Pedestal
{
	
	MacToolboxInit::MacToolboxInit()
	{
	#if !TARGET_API_MAC_CARBON
		
		::InitGraf( &qd.thePort );
		::InitFonts();
		::InitWindows();
		::InitMenus();
		::TEInit();
		::InitDialogs( NULL );
		
	#endif
		
		::InitCursor();
		// FlushEvents?
	}
	
	MemoryInit::MemoryInit( unsigned moreMasters )
	{
	#if !TARGET_API_MAC_CARBON
		
		::MaxApplZone();
		
	#endif
		
		for ( unsigned i = 0;  i < moreMasters;  ++i )
		{
			::MoreMasters();
		}
	}
	
	ApplicationContext::ApplicationContext()
	:
		itsMemoryInit( 0 )
	{
	}
	
}

