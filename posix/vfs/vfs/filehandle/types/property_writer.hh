/*
	property_writer.hh
	------------------
*/

#ifndef VFS_FILEHANDLE_TYPES_PROPERTYWRITER_HH
#define VFS_FILEHANDLE_TYPES_PROPERTYWRITER_HH

// vfs
#include "vfs/filehandle_ptr.hh"
#include "vfs/node_fwd.hh"


namespace vfs
{
	
	typedef void (*property_write_hook)( const node*       that,
	                                     const char*       begin,
	                                     const char*       end,
	                                     bool              binary );
	
	filehandle_ptr new_property_writer( const node&          file,
	                                    int                  flags,
	                                    property_write_hook  write_hook,
	                                    bool                 binary );
	
}

#endif

