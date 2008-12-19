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
			typedef unsigned long OpaqueKey;
			
			typedef OpaqueKey (*KeyHook)( const FSTree* that );
			
			typedef std::string (*ReadHook)( OpaqueKey key );
			
			KeyHook    itsKeyHook;
			ReadHook   itsReadHook;
		
		public:
			struct Undefined {};
			
			template < class Key >
			FSTree_Generated( const FSTreePtr&      parent,
			                  const std::string&    name,
			                  Key                 (*keyHook  )( const FSTree* ),
			                  std::string         (*readHook )( Key           ) )
			:
				FSTree( parent, name ),
				itsKeyHook  ( (KeyHook  ) keyHook  ),
				itsReadHook ( (ReadHook ) readHook )
			{
			}
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
}

#endif

