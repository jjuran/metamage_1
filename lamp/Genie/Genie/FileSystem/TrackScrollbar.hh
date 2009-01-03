/*	=================
 *	TrackScrollbar.hh
 *	=================
 */

#ifndef GENIE_FILESYSTEM_TRACKSCROLLBAR_HH
#define GENIE_FILESYSTEM_TRACKSCROLLBAR_HH

// Pedestal
#include "Pedestal/TrackScrollbar.hh"


namespace Genie
{
	
	class FSTree;
	
	struct Scrollbar_UserData : Pedestal::Scrollbar_UserData
	{
		const FSTree*  key;
		
		Scrollbar_UserData() : key()
		{
		}

	};
	
}

#endif

