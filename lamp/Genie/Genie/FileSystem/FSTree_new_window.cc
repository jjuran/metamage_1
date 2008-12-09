/*	====================
 *	FSTree_new_window.cc
 *	====================
 */

#include "Genie/FileSystem/FSTree_new_window.hh"

// Genie
#include "Genie/IO/Directory.hh"
#include "Genie/FileSystem/FSTree_sys_window.hh"


namespace Genie
{
	
	boost::shared_ptr< IOHandle > FSTree_new_window::OpenDirectory() const
	{
		FSTreePtr dir = NewWindow();
		
		return boost::shared_ptr< IOHandle >( new DirHandle( dir ) );
	}
	
}

