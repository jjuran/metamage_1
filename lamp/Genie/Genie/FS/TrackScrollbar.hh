/*	=================
 *	TrackScrollbar.hh
 *	=================
 */

#ifndef GENIE_FILESYSTEM_TRACKSCROLLBAR_HH
#define GENIE_FILESYSTEM_TRACKSCROLLBAR_HH

// Pedestal
#ifndef PEDESTAL_TRACKSCROLLBAR_HH
#include "Pedestal/TrackScrollbar.hh"
#endif

// Genie
#include "Genie/FS/FSTree_fwd.hh"


namespace Genie
{
	
	struct Scrollbar_UserData : Pedestal::Scrollbar_UserData
	{
		const FSTree*  key;
		
		Scrollbar_UserData() : key()
		{
		}
	};
	
}

#endif

