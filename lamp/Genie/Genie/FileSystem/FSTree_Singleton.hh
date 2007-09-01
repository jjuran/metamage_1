/*	===================
 *	FSTree_Singleton.hh
 *	===================
 */

#ifndef GENIE_FILESYSTEM_FSTREE_SINGLETON_HH
#define GENIE_FILESYSTEM_FSTREE_SINGLETON_HH

// Genie
#include "Genie/FileSystem/FSTree.hh"


namespace Genie
{
	
	template < class FSTree_Type >
	FSTreePtr GetSingleton()
	{
		static FSTreePtr singleton = FSTreePtr( new FSTree_Type() );
		
		return singleton;
	}
	
}

#endif

