/*	===================
 *	FSTree_Generated.hh
 *	===================
 */

#ifndef GENIE_FILESYSTEM_FSTREE_GENERATED_HH
#define GENIE_FILESYSTEM_FSTREE_GENERATED_HH

// Genie
#include "Genie/FileSystem/FSTree.hh"


namespace Genie
{
	
	class FSTree_Generated : public FSTree
	{
		private:
			typedef std::string (*ReadHook)( const FSTree* that );
			
			ReadHook itsReadHook;
		
		public:
			FSTree_Generated( const FSTreePtr&    parent,
			                  const std::string&  name,
			                  ReadHook            readHook )
			:
				FSTree( parent, name ),
				itsReadHook( readHook )
			{
			}
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
}

#endif

