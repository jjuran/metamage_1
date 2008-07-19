/*	===================
 *	FSTree_QueryFile.hh
 *	===================
 */

#ifndef GENIE_FILESYSTEM_FSTREE_QUERYFILE_HH
#define GENIE_FILESYSTEM_FSTREE_QUERYFILE_HH

// Genie
#include "Genie/FileSystem/FSTree.hh"
#include "Genie/IO/QueryFile.hh"


namespace Genie
{
	
	template < class Query >
	class FSTree_QueryFile : public FSTree
	{
		private:
			Query itsQuery;
		
		public:
			FSTree_QueryFile( const FSTreePtr&    parent,
			                  const std::string&  name ) : FSTree( parent, name ),
			                                               itsQuery()
			{
			}
			
			FSTree_QueryFile( const FSTreePtr&    parent,
			                  const std::string&  name,
			                  const Query&        query ) : FSTree( parent, name ),
			                                                itsQuery( query )
			{
			}
			
			off_t GetEOF() const;
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
	
	template < class Query >
	off_t FSTree_QueryFile< Query >::GetEOF() const
	{
		return itsQuery().size();
	}
	
	template < class Query >
	boost::shared_ptr< IOHandle > FSTree_QueryFile< Query >::Open( OpenFlags /*flags*/ ) const
	{
		return boost::shared_ptr< IOHandle >( new QueryFileHandle( shared_from_this(), itsQuery() ) );
	}
	
}

#endif

