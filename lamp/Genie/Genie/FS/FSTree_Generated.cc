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
	
	
	static vfs::filehandle_ptr generated_open( const FSTree* that, int flags, mode_t mode );
	
	static off_t generated_geteof( const FSTree* that );
	
	
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
		&generated_data_methods
	};
	
	
	struct generated_extra
	{
		plus::datum_storage datum;
	};
	
	
	static off_t generated_geteof( const FSTree* that )
	{
		const generated_extra& extra = *(generated_extra*) that->extra();
		
		return plus::size( extra.datum );
	}
	
	static vfs::filehandle_ptr generated_open( const FSTree* that, int flags, mode_t mode )
	{
		if ( flags != O_RDONLY )
		{
			throw p7::errno_t( EINVAL );
		}
		
		generated_extra& extra = *(generated_extra*) that->extra();
		
		plus::string& string = reinterpret_cast< plus::string& >( extra.datum );
		
		return new PropertyReaderFileHandle( *that,
		                                     flags,
		                                     string );
	}
	
	static void dispose_generated( const FSTree* that )
	{
		generated_extra& extra = *(generated_extra*) that->extra();
		
		plus::destroy( extra.datum );
	}
	
	FSTreePtr new_generated( const FSTree*        parent,
	                         const plus::string&  name,
	                         const void*          params )
	{
		Generated_ReadHook readHook = (Generated_ReadHook) params;
		
		plus::string data = readHook( parent, name );
		
		FSTree* result = new FSTree( parent,
		                             name,
		                             S_IFREG | 0400,
		                             &generated_methods,
		                             sizeof (generated_extra),
		                             &dispose_generated );
		
		generated_extra& extra = *(generated_extra*) result->extra();
		
		plus::construct_from_move( extra.datum, data.move() );
		
		return result;
	}
	
}

