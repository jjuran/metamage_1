/*	=============
 *	FSTree_Dev.hh
 *	=============
 */

#ifndef GENIE_FILESYSTEM_FSTREE_DEV_HH
#define GENIE_FILESYSTEM_FSTREE_DEV_HH

// Genie 
#include "Genie/FileSystem/FSTree_Directory.hh"


namespace Genie
{
	
	class FSTree_dev : public FSTree_Functional< void >
	{
		public:
			FSTree_dev( const FSTreePtr&    parent,
			            const std::string&  name ) : FSTree_Functional< void >( parent, name )
			{
			}
			
			void Init();
	};
	
}

#endif

