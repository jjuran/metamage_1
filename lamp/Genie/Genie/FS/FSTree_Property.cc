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


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	static IOHandle* open_for_read( const FSTree* that, int flags, bool binary )
	{
		property_params& extra = *(property_params*) that->extra();
		
		if ( extra.get == NULL )
		{
			p7::throw_errno( EACCES );
		}
		
		plus::var_string data;
		
		try
		{
			extra.get( data, that->owner(), binary );
			
			if ( !binary )
			{
				data += '\n';
			}
		}
		catch ( const undefined_property& )
		{
		}
		
		return new PropertyReaderFileHandle( that,
		                                     flags,
		                                     data );
	}
	
	static IOHandle* open_for_write( const FSTree* that, int flags, bool binary )
	{
		property_params& extra = *(property_params*) that->extra();
		
		if ( extra.set == NULL )
		{
			p7::throw_errno( EACCES );
		}
		
		return new PropertyWriterFileHandle( that,
		                                     flags,
		                                     extra.set,
		                                     binary );
	}
	
	static IOPtr property_open( const FSTree* that, int flags, mode_t mode )
	{
		IOHandle* result = NULL;
		
		const char* name = that->name().data();
		
		const bool binary = name[0] == '.'  &&  name[1] == '~';
		
		if ( flags == O_RDONLY )
		{
			result = open_for_read( that, flags, binary );
		}
		else if ( (flags & ~O_CREAT) == (O_WRONLY | O_TRUNC) )
		{
			result = open_for_write( that, flags, binary );
		}
		else
		{
			throw p7::errno_t( EINVAL );
		}
		
		return result;
	}
	
	static off_t property_geteof( const FSTree* that )
	{
		property_params& extra = *(property_params*) that->extra();
		
		const char* name = that->name().data();
		
		const bool binary = name[0] == '.'  &&  name[1] == '~';
		
		if ( (binary  &&  extra.size != 0)  ||  extra.get == NULL )
		{
			return extra.size;
		}
		
		plus::var_string data;
		
		try
		{
			extra.get( data, that->owner(), binary );
		}
		catch ( const undefined_property& )
		{
		}
		
		return data.size() + !binary;
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
	
	FSTreePtr new_property( const FSTree*        parent,
	                        const plus::string&  name,
	                        const void*          params_ )
	{
		const property_params& params = *(const property_params*) params_;
		
		const mode_t mode = get_property_filemode( params.get,
		                                           params.set,
		                                           parent );
		
		FSTree* result = new FSTree( parent,
		                             name,
		                             mode,
		                             &property_methods,
		                             sizeof (property_params) );
		
		property_params& extra = *(property_params*) result->extra();
		
		extra = params;
		
		return result;
	}
	
}

