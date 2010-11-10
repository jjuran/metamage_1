/*	===================
 *	FSTree_Generated.cc
 *	===================
 */

#include "Genie/FS/FSTree_Generated.hh"

// POSIX
#include <fcntl.h>

// Debug
#include "debug/assert.hh"

// poseven
#include "poseven/types/errno_t.hh"

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
			FSTree_Generated( const FSTreePtr&     parent,
			                  const plus::string&  name,
			                  ReadHook             readHook )
			:
				FSTree( parent, name ),
				itsReadHook( readHook )
			{
				ASSERT( readHook != NULL );
			}
			
			bool Exists() const;
			
			off_t GetEOF() const;
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
	
	bool FSTree_Generated::Exists() const
	{
		try
		{
			(void) itsReadHook( this );
			
			return true;
		}
		catch ( ... )
		{
		}
		
		return false;
	}
	
	off_t FSTree_Generated::GetEOF() const
	{
		const plus::string data = itsReadHook( this );
		
		return data.size();
	}
	
	boost::shared_ptr< IOHandle > FSTree_Generated::Open( OpenFlags flags ) const
	{
		if ( flags != O_RDONLY )
		{
			throw p7::errno_t( EINVAL );
		}
		
		plus::string data = itsReadHook( this );
		
		return seize_ptr( new PropertyReaderFileHandle( Self(),
		                                                flags,
		                                                data ) );
	}
	
	FSTreePtr New_FSTree_Generated( const FSTreePtr&     parent,
	                                const plus::string&  name,
	                                Generated_ReadHook   readHook )
	{
		return seize_ptr( new FSTree_Generated( parent, name, readHook ) );
	}
	
}

