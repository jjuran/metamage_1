/*	=================
 *	TrackScrollbar.hh
 *	=================
 */

#ifndef GENIE_FILESYSTEM_TRACKSCROLLBAR_HH
#define GENIE_FILESYSTEM_TRACKSCROLLBAR_HH

// vfs
#include "vfs/node_fwd.hh"

// Pedestal
#ifndef PEDESTAL_TRACKSCROLLBAR_HH
#include "Pedestal/TrackScrollbar.hh"
#endif


namespace Genie
{
	
	struct Scrollbar_UserData : Pedestal::Scrollbar_UserData
	{
		const vfs::node*  key;
		
		Scrollbar_UserData() : key()
		{
		}
	};
	
}

#endif
