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
	
	class FSTree_sys_mac_user_home : public FSTree
	{
		public:
			FSTree_sys_mac_user_home( const FSTreePtr&    parent,
			                          const std::string&  name ) : FSTree( parent, name )
			{
			}
			
			bool IsLink() const  { return true; }
			
			std::string ReadLink() const;
			
			FSTreePtr ResolveLink() const;
	};
	
}

#endif

