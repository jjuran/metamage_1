/*	==============
 *	FSTree_Null.hh
 *	==============
 */

#ifndef GENIE_FILESYSTEM_FSTREE_NULL_HH
#define GENIE_FILESYSTEM_FSTREE_NULL_HH

// Genie
#include "Genie/FileSystem/FSTree_Singleton.hh"


namespace Genie
{
	
	class FSTree_Null : public FSTree
	{
		public:
			bool Exists() const  { return false; }
			bool IsFile() const  { return false; }
			
			FSTreePtr Parent() const;
	};
	
	FSTreePtr FSNull();
	
}

#endif

