/*	===================
 *	FSTree_Generated.cc
 *	===================
 */

#include "Genie/FS/FSTree_Generated.hh"

// POSIX
#include <fcntl.h>

// POSeven
#include "POSeven/Errno.hh"

// Genie
#include "Genie/IO/PropertyFile.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	class FSTree_Generated : public FSTree
	{
		private:
			typedef Generated_ReadHook ReadHook;
			
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
			
			bool Exists() const;
			
			off_t GetEOF() const;
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
	
	bool FSTree_Generated::Exists() const
	{
		if ( itsReadHook != NULL )
		{
			try
			{
				(void) itsReadHook( this );
				
				return true;
			}
			catch ( ... )
			{
			}
		}
		
		return false;
	}
	
	off_t FSTree_Generated::GetEOF() const
	{
		if ( itsReadHook == NULL )
		{
			p7::throw_errno( EACCES );
		}
		
		const std::string data = itsReadHook( this );
		
		return data.size();
	}
	
	boost::shared_ptr< IOHandle > FSTree_Generated::Open( OpenFlags flags ) const
	{
		if ( flags != O_RDONLY )
		{
			throw p7::errno_t( EINVAL );
		}
		
		if ( itsReadHook == NULL )
		{
			p7::throw_errno( EACCES );
		}
		
		std::string data = itsReadHook( this );
		
		return boost::shared_ptr< IOHandle >( new PropertyReaderFileHandle( Self(),
		                                                                    flags,
		                                                                    data ) );
	}
	
	FSTreePtr New_FSTree_Generated( const FSTreePtr&    parent,
	                                const std::string&  name,
	                                Generated_ReadHook  readHook )
	{
		return FSTreePtr( new FSTree_Generated( parent, name, readHook ) );
	}
	
}

