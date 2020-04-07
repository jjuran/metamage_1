/*
	property_writer.cc
	------------------
*/

#include "vfs/filehandle/types/property_writer.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/node.hh"
#include "vfs/filehandle/methods/stream_method_set.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/primitives/get_file.hh"


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	typedef void (*named_property_write_hook)( const node*          that,
	                                           const char*          begin,
	                                           const char*          end,
	                                           bool                 binary,
	                                           const plus::string&  name );
	
	struct property_writer_extra
	{
		property_write_hook  write_hook;
		bool                 binary;
	};
	
	static inline
	void call_write_hook( property_write_hook  hook,
	                      const node*          that,
	                      const char*          begin,
	                      const char*          end,
	                      bool                 binary )
	{
		named_property_write_hook write = (named_property_write_hook) hook;
		
		write( that->owner(), begin, end, binary, that->name() );
	}
	
	static ssize_t propertywriter_write( filehandle* that, const char* buffer, size_t n )
	{
		property_writer_extra& extra = *(property_writer_extra*) that->extra();
		
		if ( n == 0 )
		{
			throw p7::errno_t( EINVAL );
		}
		
		size_t length = n;
		
		if ( !extra.binary  &&  buffer[ --length ] != '\n' )
		{
			throw p7::errno_t( EINVAL );
		}
		
		call_write_hook( extra.write_hook,
		                 get_file( *that ).get(),
		                 buffer,
		                 buffer + length,
		                 extra.binary );
		
		return n;
	}
	
	static const stream_method_set propertywriter_stream_methods =
	{
		NULL,
		NULL,
		&propertywriter_write,
	};
	
	static const filehandle_method_set propertywriter_methods =
	{
		NULL,
		NULL,
		&propertywriter_stream_methods,
	};
	
	
	filehandle_ptr new_property_writer( const node&          file,
	                                    int                  flags,
	                                    property_write_hook  write_hook,
	                                    bool                 binary )
	{
		filehandle* result = new filehandle( &file,
		                                     flags,
		                                     &propertywriter_methods,
		                                     sizeof (property_writer_extra) );
		
		property_writer_extra& extra = *(property_writer_extra*) result->extra();
		
		extra.write_hook = write_hook;
		extra.binary     = binary;
		
		return result;
	}
	
}
