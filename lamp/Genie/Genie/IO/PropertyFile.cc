/*	===============
 *	PropertyFile.cc
 *	===============
 */

#include "Genie/IO/PropertyFile.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/filehandle/types/property_writer.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	PropertyWriterFileHandle::PropertyWriterFileHandle( const vfs::node&  file,
	                                                    int               flags,
	                                                    WriteHook         writeHook,
	                                                    bool              binary )
	:
		StreamHandle( &file, flags ),
		itsWriteHook( writeHook ),
		itIsBinary( binary )
	{
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

namespace vfs
{
	
	filehandle_ptr new_property_writer( const node&          file,
	                                    int                  flags,
	                                    property_write_hook  write_hook,
	                                    bool                 binary )
	{
		return new Genie::PropertyWriterFileHandle( file, flags, write_hook, binary );
	}
	
}

