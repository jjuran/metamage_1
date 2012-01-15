/*
	CreatableSymLink.cc
	-------------------
*/

#include "Genie/FS/CreatableSymLink.hh"

// Genie
#include "Genie/FS/FSTree.hh"


namespace Genie
{
	
	class FSTree_CreatableSymLink : public FSTree
	{
		private:
			symlinker its_symlink;
		
		public:
			FSTree_CreatableSymLink( const FSTreePtr&     parent,
			                         const plus::string&  name,
			                         symlinker            symlink );
			
			void SymLink( const plus::string& target ) const;
	};
	
	
	FSTree_CreatableSymLink::FSTree_CreatableSymLink( const FSTreePtr&     parent,
	                                                  const plus::string&  name,
	                                                  symlinker            symlink )
	:
		FSTree( parent, name, 0 ),
		its_symlink( symlink )
	{
	}
	
	void FSTree_CreatableSymLink::SymLink( const plus::string& target ) const
	{
		its_symlink( this, target );
	}
	
	FSTreePtr New_CreatableSymLink( const FSTreePtr&     parent,
	                                const plus::string&  name,
	                                symlinker            symlink )
	{
		return new FSTree_CreatableSymLink( parent, name, symlink );
	}
	
}

