/*
	property_reader.cc
	------------------
*/

#include "vfs/filehandle/types/property_reader.hh"

// more-libc
#include "more/string.h"

// plus
#include "plus/string.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/filehandle/methods/bstore_method_set.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"


namespace vfs
{
	
	struct property_reader_extra
	{
		plus::datum_storage  value;
	};
	
	static ssize_t propertyreader_pread( filehandle* file, char* buffer, size_t n, off_t offset )
	{
		property_reader_extra& extra = *(property_reader_extra*) file->extra();
		
		const size_t length = size( extra.value );
		
		if ( offset >= length )
		{
			return 0;
		}
		
		if ( n > length - offset )
		{
			n = length - offset;
		}
		
		mempcpy( buffer, begin( extra.value ) + offset, n );
		
		return n;
	}
	
	static off_t propertyreader_geteof( filehandle* file )
	{
		property_reader_extra& extra = *(property_reader_extra*) file->extra();
		
		return size( extra.value );
	}
	
	static const bstore_method_set propertyreader_bstore_methods =
	{
		&propertyreader_pread,
		&propertyreader_geteof,
	};
	
	static const filehandle_method_set propertyreader_methods =
	{
		&propertyreader_bstore_methods,
	};
	
	static void dispose_property_reader( filehandle* that )
	{
		property_reader_extra& extra = *(property_reader_extra*) that->extra();
		
		destroy( extra.value );
	}
	
	
	filehandle_ptr new_property_reader( const node&          file,
	                                    int                  flags,
	                                    const plus::string&  value )
	{
		plus::string value_copy = value;
		
		filehandle* result = new filehandle( &file,
		                                     flags,
		                                     &propertyreader_methods,
		                                     sizeof (property_reader_extra),
		                                     &dispose_property_reader );
		
		property_reader_extra& extra = *(property_reader_extra*) result->extra();
		
		construct_from_move( extra.value, value_copy.move() );
		
		return result;
	}
	
}
