/*	==================
 *	FSTree_Property.cc
 *	==================
 */

#include "Genie/FS/FSTree_Property.hh"

// POSIX
#include <fcntl.h>
#include <sys/stat.h>

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/FS/data_method_set.hh"
#include "Genie/FS/node_method_set.hh"
#include "Genie/FS/property.hh"
#include "Genie/IO/PropertyFile.hh"


#ifndef O_BINARY
#define O_BINARY  0
#endif


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	class FSTree_Property : public FSTree
	{
		public:
			FSTree_Property( const FSTreePtr&     parent,
			                 const plus::string&  name,
			                 mode_t               mode );
	};
	
	
	static IOHandle* open_for_read( const FSTree* node, int flags )
	{
		property_params& extra = *(property_params*) node->extra();
		
		if ( extra.get == NULL )
		{
			p7::throw_errno( EACCES );
		}
		
		plus::var_string data;
		
		try
		{
			const bool binary = flags & O_BINARY;
			
			extra.get( data, node->owner(), binary );
			
			if ( !binary )
			{
				data += '\n';
			}
		}
		catch ( const undefined_property& )
		{
		}
		
		return new PropertyReaderFileHandle( node,
		                                     flags,
		                                     data );
	}
	
	static IOHandle* open_for_write( const FSTree* node, int flags )
	{
		property_params& extra = *(property_params*) node->extra();
		
		if ( extra.set == NULL )
		{
			p7::throw_errno( EACCES );
		}
		
		const bool binary = flags & O_BINARY;
		
		return new PropertyWriterFileHandle( node,
		                                     flags,
		                                     extra.set,
		                                     binary );
	}
	
	static IOPtr property_open( const FSTree* node, int flags, mode_t mode )
	{
		IOHandle* result = NULL;
		
		if ( (flags & ~O_BINARY) == O_RDONLY )
		{
			result = open_for_read( node, flags );
		}
		else if ( (flags & ~(O_CREAT | O_BINARY)) == (O_WRONLY | O_TRUNC) )
		{
			result = open_for_write( node, flags );
		}
		else
		{
			throw p7::errno_t( EINVAL );
		}
		
		return result;
	}
	
	static off_t property_geteof( const FSTree* node )
	{
		property_params& extra = *(property_params*) node->extra();
		
		if ( extra.size != 0  ||  extra.get == NULL )
		{
			return extra.size;
		}
		
		plus::var_string data;
		
		try
		{
			const bool binary = true;  // Return binary length
			
			extra.get( data, node->owner(), binary );
		}
		catch ( const undefined_property& )
		{
		}
		
		return data.size();
	}
	
	static const data_method_set property_data_methods =
	{
		&property_open,
		&property_geteof
	};
	
	static const node_method_set property_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		&property_data_methods
	};
	
	
	static mode_t get_property_filemode( property_get_hook  get_hook,
	                                     property_set_hook  set_hook,
	                                     const FSTree*      parent )
	{
		mode_t result = S_IFREG | (set_hook ? S_IWUSR : 0);
		
		if ( get_hook != NULL )
		{
			try
			{
				// Binary is probably more efficient, but only slightly so
				const bool binary_vs_text = true;
				
				plus::var_string data;
				
				get_hook( data, parent, binary_vs_text );
				
				result |= S_IRUSR;
			}
			catch ( ... )
			{
				return 0;  // failed read => nonexistent
			}
		}
		
		return result;
	}
	
	FSTreePtr new_property( const FSTreePtr&     parent,
	                        const plus::string&  name,
	                        const void*          params_ )
	{
		const property_params& params = *(const property_params*) params_;
		
		const mode_t mode = get_property_filemode( params.get,
		                                           params.set,
		                                           parent.get() );
		
		FSTree* result = new FSTree_Property( parent, name, mode );
		
		property_params& extra = *(property_params*) result->extra();
		
		extra = params;
		
		return result;
	}
	
	FSTree_Property::FSTree_Property( const FSTreePtr&     parent,
	                                  const plus::string&  name,
	                                  mode_t               mode )
	:
		FSTree( parent,
		        name,
		        mode,
		        &property_methods,
		        sizeof (property_params) )
	{
	}
	
}

