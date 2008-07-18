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
	const FSTreePtr& MakeSingleton( const FSTreePtr& parent )
	{
		FSTree_Type* raw_ptr = NULL;
		
		static FSTreePtr singleton = FSTreePtr( raw_ptr = new FSTree_Type() );
		
		if ( raw_ptr != NULL )
		{
			raw_ptr->Init();
		}
		
		return singleton;
	}
	
	template < class FSTree_Type >
	FSTreePtr GetSingleton( const FSTreePtr& parent = FSTreePtr() )
	{
		static const FSTreePtr& singleton = MakeSingleton< FSTree_Type >( parent );
		
		return singleton;
	}
	
}

#endif

