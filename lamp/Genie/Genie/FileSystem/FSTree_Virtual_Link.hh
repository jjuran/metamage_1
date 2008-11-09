/*	======================
 *	FSTree_Virtual_Link.hh
 *	======================
 */

#ifndef GENIE_FILESYSTEM_FSTREEVIRTUALLINK_HH
#define GENIE_FILESYSTEM_FSTREEVIRTUALLINK_HH

// Genie
#include "Genie/FileSystem/FSTree.hh"


namespace Genie
{
	
	class FSTree_Virtual_Link : public FSTree
	{
		private:
			std::string itsTarget;
		
		public:
			FSTree_Virtual_Link( const FSTreePtr&    parent,
			                     const std::string&  name,
			                     const std::string&  target ) : FSTree( parent, name ),
			                                                    itsTarget( target )
			{
			}
			
			bool IsLink() const  { return true; }
			
			std::string ReadLink() const  { return itsTarget; }
			
			FSTreePtr ResolveLink() const;
	};
	
}

#endif

