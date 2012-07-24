/*
	property_file.cc
	----------------
*/

#include "vfs/node/types/property_file.hh"

// POSIX
#include <fcntl.h>
#include <sys/stat.h>

// plus
#include "plus/var_string.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/node.hh"
#include "vfs/property.hh"
#include "vfs/filehandle/types/property_reader.hh"
#include "vfs/filehandle/types/property_writer.hh"
#include "vfs/methods/data_method_set.hh"
#include "vfs/methods/node_method_set.hh"


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	typedef void (*named_property_get_hook)( plus::var_string&    out,
	                                         const node*          that,
	                                         bool                 binary,
	                                         const plus::string&  name );
	
	static filehandle_ptr open_for_read( const node& that, int flags, bool binary )
	{
		property_params& extra = *(property_params*) that.extra();
		
		if ( extra.get == NULL )
		{
			p7::throw_errno( EACCES );
		}
		
		plus::var_string data;
		
		try
		{
			named_property_get_hook get = (named_property_get_hook) extra.get;
			
			get( data, that.owner(), binary, that.name() );
			
			if ( !binary )
			{
				data += '\n';
			}
		}
		catch ( const undefined_property& )
		{
		}
		
		return new_property_reader( that, flags, data );
	}
	
	static filehandle_ptr open_for_write( const node& that, int flags, bool binary )
	{
		property_params& extra = *(property_params*) that.extra();
		
		if ( extra.set == NULL )
		{
			p7::throw_errno( EACCES );
		}
		
		return new_property_writer( that, flags, extra.set, binary );
	}
	
	static filehandle_ptr property_open( const node* that, int flags, mode_t mode )
	{
		const char* name = that->name().data();
		
		const bool binary = name[0] == '.'  &&  name[1] == '~';
		
		const int fewer_flags = flags & ~O_NONBLOCK;
		
		if ( fewer_flags == O_RDONLY )
		{
			return open_for_read( *that, flags, binary );
		}
		else if ( (fewer_flags & ~O_CREAT) == (O_WRONLY | O_TRUNC) )
		{
			// return below to silence Metrowerks warning
		}
		else
		{
			throw p7::errno_t( EINVAL );
		}
		
		return open_for_write( *that, flags, binary );
	}
	
	static off_t property_geteof( const node* that )
	{
		property_params& extra = *(property_params*) that->extra();
		
		const char* name = that->name().data();
		
		// Only return a (positive) fixed size in binary mode
		
		if ( extra.size >= 0  &&  name[0] == '.'  &&  name[1] == '~' )
		{
			return extra.size;
		}
		
		return 0;
	}
	
	static const data_method_set property_data_methods =
	{
		&property_open,
		&property_geteof
	};
	
	static const node_method_set property_methods =
	{
		NULL,
		&property_data_methods
	};
	
	
	static mode_t get_property_filemode( property_get_hook    get_hook,
	                                     property_set_hook    set_hook,
	                                     const node*          parent,
	                                     const plus::string&  name )
	{
		mode_t result = S_IFREG | (set_hook ? S_IWUSR : 0);
		
		named_property_get_hook get = (named_property_get_hook) get_hook;
		
		if ( get_hook != NULL )
		{
			try
			{
				// Binary is probably more efficient, but only slightly so
				const bool binary_vs_text = true;
				
				plus::var_string data;
				
				get( data, parent, binary_vs_text, name );
				
				result |= 0444;  // all-readable
			}
			catch ( ... )
			{
				return 0;  // failed read => nonexistent
			}
		}
		
		return result;
	}
	
	node_ptr new_property( const node*          parent,
	                       const plus::string&  name,
	                       const void*          params_ )
	{
		const property_params& params = *(const property_params*) params_;
		
		const mode_t mode = get_property_filemode( params.get,
		                                           params.set,
		                                           parent,
		                                           name );
		
		node* result = new node( parent,
		                         name,
		                         mode,
		                         &property_methods,
		                         sizeof (property_params) );
		
		property_params& extra = *(property_params*) result->extra();
		
		extra = params;
		
		return result;
	}
	
}
