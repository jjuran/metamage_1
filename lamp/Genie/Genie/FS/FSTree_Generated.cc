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
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/data_method_set.hh"
#include "Genie/FS/node_method_set.hh"
#include "Genie/IO/PropertyFile.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	static IOPtr generated_open( const FSTree* node, int flags, mode_t mode );
	
	static off_t generated_geteof( const FSTree* node );
	
	
	static const data_method_set generated_data_methods =
	{
		&generated_open,
		&generated_geteof
	};
	
	static const node_method_set generated_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		&generated_data_methods
	};
	
	
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
				FSTree( parent, name, S_IFREG | 0400, &generated_methods ),
				its_data( readHook( parent.get(), name ) )
			{
			}
			
			const plus::string& data() const  { return its_data; }
	};
	
	
	static off_t generated_geteof( const FSTree* node )
	{
		const FSTree_Generated* file = static_cast< const FSTree_Generated* >( node );
		
		return file->data().size();
	}
	
	static IOPtr generated_open( const FSTree* node, int flags, mode_t mode )
	{
		if ( flags != O_RDONLY )
		{
			throw p7::errno_t( EINVAL );
		}
		
		const FSTree_Generated* file = static_cast< const FSTree_Generated* >( node );
		
		return new PropertyReaderFileHandle( node,
		                                     flags,
		                                     file->data() );
	}
	
	FSTreePtr new_generated( const FSTreePtr&     parent,
	                         const plus::string&  name,
	                         const void*          params )
	{
		Generated_ReadHook readHook = (Generated_ReadHook) params;
		
		return new FSTree_Generated( parent, name, readHook );
	}
	
}

