/*	======================
 *	FSTree_Virtual_Link.cc
 *	======================
 */

#include "Genie/FS/FSTree_Virtual_Link.hh"

// Genie
#include "Genie/FS/ResolvePathname.hh"


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
	
	
	FSTreePtr FSTree_Virtual_Link::ResolveLink() const
	{
		return ResolveAbsolutePath( itsTarget );
	}
	
	FSTreePtr New_FSTree_Virtual_Link( const FSTreePtr&    parent,
	                                   const std::string&  name,
	                                   const std::string&  target )
	{
		return FSTreePtr( new FSTree_Virtual_Link( parent, name, target ) );
	}
	
}

