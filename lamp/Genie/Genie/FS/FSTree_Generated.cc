/*	===================
 *	FSTree_Generated.cc
 *	===================
 */

#include "Genie/FS/FSTree_Generated.hh"

// POSIX
#include <fcntl.h>
#include <sys/stat.h>

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
			
			plus::string its_data;
		
		public:
			FSTree_Generated( const FSTreePtr&     parent,
			                  const plus::string&  name,
			                  ReadHook             readHook )
			:
				FSTree( parent, name, S_IFREG | 0400 ),
				its_data( readHook( parent.get(), name ) )
			{
			}
			
			off_t GetEOF() const;
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
	
	off_t FSTree_Generated::GetEOF() const
	{
		return its_data.size();
	}
	
	boost::shared_ptr< IOHandle > FSTree_Generated::Open( OpenFlags flags ) const
	{
		if ( flags != O_RDONLY )
		{
			throw p7::errno_t( EINVAL );
		}
		
		return seize_ptr( new PropertyReaderFileHandle( Self(),
		                                                flags,
		                                                its_data ) );
	}
	
	FSTreePtr New_FSTree_Generated( const FSTreePtr&     parent,
	                                const plus::string&  name,
	                                Generated_ReadHook   readHook )
	{
		return seize_ptr( new FSTree_Generated( parent, name, readHook ) );
	}
	
}

