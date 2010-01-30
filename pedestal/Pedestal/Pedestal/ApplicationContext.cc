/*	=====================
 *	ApplicationContext.cc
 *	=====================
 */

#include "Pedestal/ApplicationContext.hh"

// Mac OS
#include <Dialogs.h>


namespace Pedestal
{
	
	void Init_MacToolbox()
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
	
	void Init_Memory( unsigned moreMasters )
	{
	#if !TARGET_API_MAC_CARBON
		
		::MaxApplZone();
		
	#endif
		
		for ( unsigned i = 0;  i < moreMasters;  ++i )
		{
			::MoreMasters();
		}
	}
	
}

