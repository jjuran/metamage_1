/*	===========
 *	AboutBox.cc
 *	===========
 */

#include "Pedestal/AboutBox.hh"

// mac-app-utils
#include "mac_app/about_box.hh"

// Pedestal
#include "Pedestal/OwnerResource.hh"


namespace Pedestal
{
	
	void ShowAboutBox()
	{
		const OSType creator = GetCreatorFromBNDL();
		
		mac::app::show_About_box( creator );
	}
	
}
