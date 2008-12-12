/*	====================
 *	FSTree_PseudoFile.hh
 *	====================
 */

#ifndef GENIE_FILESYSTEM_FSTREE_PSEUDOFILE_HH
#define GENIE_FILESYSTEM_FSTREE_PSEUDOFILE_HH

// POSIX
#include <fcntl.h>

// Genie
#include "Genie/FileSystem/FSTree.hh"
#include "Genie/IO/PseudoFile.hh"
#include "Genie/IO/QueryFile.hh"


namespace Genie
{
	
	template < class Property >
	class FSTree_PseudoFile : public FSTree
	{
		private:
			Property itsProperty;
		
		public:
			FSTree_PseudoFile( const FSTreePtr&    parent,
			                   const std::string&  name ) : FSTree( parent, name ),
			                                                itsProperty()
			{
			}
			
			FSTree_PseudoFile( const FSTreePtr&    parent,
			                   const std::string&  name,
			                   const Property&     property ) : FSTree( parent, name ),
			                                                    itsProperty( property )
			{
			}
			
			mode_t FilePermMode() const;
			
			off_t GetEOF() const;
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
	
	template < class Property >
	mode_t FSTree_PseudoFile< Property >::FilePermMode() const
	{
		return S_IRUSR | S_IWUSR;
	}
	
	template < class Property >
	off_t FSTree_PseudoFile< Property >::GetEOF() const
	{
		return itsProperty.Get().size();
	}
	
	template < class Property >
	boost::shared_ptr< IOHandle > FSTree_PseudoFile< Property >::Open( OpenFlags flags ) const
	{
		IOHandle* result = NULL;
		
		if ( flags == O_RDONLY )
		{
			result = new QueryFileHandle( shared_from_this(),
			                              flags,
			                              itsProperty.Get() );
		}
		else if ( (flags & ~O_CREAT) == (O_WRONLY | O_TRUNC) )
		{
			result = new PseudoFileHandle< Property >( shared_from_this(),
			                                           flags,
			                                           itsProperty );
		}
		else
		{
			throw poseven::errno_t( EINVAL );
		}
		
		return boost::shared_ptr< IOHandle >( result );
	}
	
}

#endif

