/*	=========================
 *	FSTree_sys_mac_gestalt.hh
 *	=========================
 */

#ifndef GENIE_FILESYSTEM_FSTREESYSMACGESTALT_HH
#define GENIE_FILESYSTEM_FSTREESYSMACGESTALT_HH

// POSIX
#include "sys/stat.h"

// Genie
#include "Genie/FileSystem/FSTree.hh"


namespace Genie
{
	
	class FSTree_sys_mac_gestalt : public FSTree
	{
		public:
			FSTree_sys_mac_gestalt( const FSTreePtr& parent ) : FSTree( parent )
			{
			}
			
			std::string Name() const  { return "gestalt"; }
			
			mode_t FileTypeMode() const  { return S_IFCHR; }
			mode_t FilePermMode() const  { return S_IRUSR; }
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
}

#endif

