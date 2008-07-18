/*	===========================
 *	FSTree_sys_mac_user_home.hh
 *	===========================
 */

#ifndef GENIE_FILESYSTEM_FSTREESYSMACUSERHOME_HH
#define GENIE_FILESYSTEM_FSTREESYSMACUSERHOME_HH

// Mac OS Universal Interfaces
#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

// POSIX
#include "sys/stat.h"

// Genie
#include "Genie/FileSystem/FSTree.hh"


namespace Genie
{
	
	void CheckForHomeFolder();
	
	class FSTree_sys_mac_user_home : public FSTree
	{
		public:
			FSTree_sys_mac_user_home( const FSTreePtr& parent ) : FSTree( parent )
			{
			}
			
			bool IsLink() const  { return true; }
			
			std::string Name() const  { return "home"; }
			
			std::string ReadLink() const;
			
			FSTreePtr ResolveLink() const;
	};
	
}

#endif

