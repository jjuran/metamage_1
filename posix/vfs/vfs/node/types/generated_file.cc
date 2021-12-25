/*
	generated_file.cc
	-----------------
*/

#include "vfs/node/types/generated_file.hh"

// POSIX
#include <fcntl.h>
#include <sys/stat.h>

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/node.hh"
#include "vfs/filehandle/types/property_reader.hh"
#include "vfs/methods/data_method_set.hh"
#include "vfs/methods/node_method_set.hh"


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	static filehandle_ptr generated_open( const node* that, int flags, mode_t mode );
	
	static off_t generated_geteof( const node* that );
	
	
	static const data_method_set generated_data_methods =
	{
		&generated_open,
		&generated_geteof
	};
	
	static const node_method_set generated_methods =
	{
		NULL,
		&generated_data_methods
	};
	
	
	struct generated_extra
	{
		vxo::datum_storage datum;
	};
	
	
	static off_t generated_geteof( const node* that )
	{
		const generated_extra& extra = *(generated_extra*) that->extra();
		
		return vxo::size( extra.datum );
	}
	
	static filehandle_ptr generated_open( const node* that, int flags, mode_t mode )
	{
		if ( flags != O_RDONLY )
		{
			throw p7::errno_t( EINVAL );
		}
		
		generated_extra& extra = *(generated_extra*) that->extra();
		
		plus::string& string = reinterpret_cast< plus::string& >( extra.datum );
		
		return new_property_reader( *that, flags, string );
	}
	
	static void dispose_generated( const node* that )
	{
		generated_extra& extra = *(generated_extra*) that->extra();
		
		vxo::destroy( extra.datum );
	}
	
	node_ptr new_generated( const node*          parent,
	                        const plus::string&  name,
	                        const void*          params )
	{
		generated_file_hook read_hook = (generated_file_hook) params;
		
		plus::string data = read_hook( parent, name );
		
		node* result = new node( parent,
		                         name,
		                         S_IFREG | 0400,
		                         &generated_methods,
		                         sizeof (generated_extra),
		                         &dispose_generated );
		
		generated_extra& extra = *(generated_extra*) result->extra();
		
		vxo::construct_from_move( extra.datum, data.move() );
		
		return result;
	}
	
}
