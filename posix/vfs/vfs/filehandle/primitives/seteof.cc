/*
	seteof.cc
	---------
*/

#include "vfs/filehandle/primitives/seteof.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/node.hh"
#include "vfs/filehandle/methods/bstore_method_set.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/primitives/seteof.hh"


namespace vfs
{
	
	void seteof( filehandle& that, off_t length )
	{
		const vfs::bstore_method_set& bstore_methods = that.bstore_methods();
		
		if ( bstore_methods.seteof != NULL )
		{
			bstore_methods.seteof( &that, length );
		}
		else
		{
			seteof( *that.GetFile(), length );
		}
	}
	
}

