/*	=====================
 *	FSTree_dev_gestalt.hh
 *	=====================
 */

#ifndef GENIE_FILESYSTEM_FSTREEDEVGESTALT_HH
#define GENIE_FILESYSTEM_FSTREEDEVGESTALT_HH

// POSIX
#include "sys/stat.h"

// Genie
#include "Genie/FS/FSTree.hh"


namespace Genie
{
	
	class FSTree_dev_gestalt : public FSTree
	{
		public:
			FSTree_dev_gestalt( const FSTreePtr&    parent,
			                    const std::string&  name ) : FSTree( parent, name )
			{
			}
			
			mode_t FileTypeMode() const  { return S_IFCHR; }
			mode_t FilePermMode() const  { return S_IRUSR; }
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
}

#endif

