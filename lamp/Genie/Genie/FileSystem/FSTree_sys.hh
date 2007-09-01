/*	=============
 *	FSTree_sys.hh
 *	=============
 */

#ifndef GENIE_FILESYSTEM_FSTREE_SYS_HH
#define GENIE_FILESYSTEM_FSTREE_SYS_HH

// Genie
#include "Genie/FileSystem/FSTree_Directory.hh"


namespace Genie
{
	
	class FSTree_sys : public FSTree_Virtual
	{
		public:
			FSTree_sys();
			
			std::string Name() const  { return "sys"; }
			
			FSTreePtr Parent() const  { return FSRoot(); }
	};
	
}

#endif

