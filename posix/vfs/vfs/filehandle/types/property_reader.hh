/*
	property_reader.hh
	------------------
*/

#ifndef VFS_FILEHANDLE_TYPES_PROPERTYREADER_HH
#define VFS_FILEHANDLE_TYPES_PROPERTYREADER_HH

// plus
#include "plus/string_fwd.hh"

// vfs
#include "vfs/filehandle_ptr.hh"
#include "vfs/node_fwd.hh"


namespace vfs
{
	
	filehandle_ptr new_property_reader( const node&          file,
	                                    int                  flags,
	                                    const plus::string&  value );
	
}

#endif

