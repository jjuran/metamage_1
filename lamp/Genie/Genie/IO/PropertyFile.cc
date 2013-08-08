/*	===============
 *	PropertyFile.cc
 *	===============
 */

#include "Genie/IO/PropertyFile.hh"

// Standard C++
#include <algorithm>

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/filehandle/methods/bstore_method_set.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	static ssize_t propertyreader_pread( vfs::filehandle* file, char* buffer, size_t n, off_t offset )
	{
		return static_cast< PropertyReaderFileHandle& >( *file ).Positioned_Read( buffer, n, offset );
	}
	
	static off_t propertyreader_geteof( vfs::filehandle* file )
	{
		return static_cast< PropertyReaderFileHandle& >( *file ).GetEOF();
	}
	
	static const vfs::bstore_method_set propertyreader_bstore_methods =
	{
		&propertyreader_pread,
		&propertyreader_geteof,
	};
	
	static const vfs::filehandle_method_set propertyreader_methods =
	{
		&propertyreader_bstore_methods,
	};
	
	
	PropertyReaderFileHandle::PropertyReaderFileHandle( const FSTreePtr&     file,
	                                                    int                  flags,
	                                                    const plus::string&  value )
	:
		RegularFileHandle( file, flags, &propertyreader_methods ),
		itsData( value )
	{
	}
	
	IOPtr PropertyReaderFileHandle::Clone()
	{
		return new PropertyReaderFileHandle( GetFile(), GetFlags(), itsData );
	}
	
	ssize_t PropertyReaderFileHandle::Positioned_Read( char* buffer, size_t n_bytes, off_t offset )
	{
		if ( offset >= itsData.size() )
		{
			return 0;
		}
		
		n_bytes = std::min< size_t >( n_bytes, itsData.size() - offset );
		
		memcpy( buffer, &itsData[ offset ], n_bytes );
		
		return n_bytes;
	}
	
	
	ssize_t PropertyWriterFileHandle::SysWrite( const char* buffer, std::size_t byteCount )
	{
		if ( byteCount == 0 )
		{
			throw p7::errno_t( EINVAL );
		}
		
		std::size_t length = byteCount;
		
		if ( !itIsBinary  &&  buffer[ --length ] != '\n' )
		{
			throw p7::errno_t( EINVAL );
		}
		
		itsWriteHook( GetFile()->owner(), buffer, buffer + length, itIsBinary );
		
		return byteCount;
	}
	
}

