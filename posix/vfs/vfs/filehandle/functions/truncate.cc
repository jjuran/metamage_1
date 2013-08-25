/*
	truncate.cc
	----------
*/

#include "vfs/filehandle/functions/truncate.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/primitives/seteof.hh"


namespace vfs
{
	
	void truncate( filehandle& that )
	{
		if ( that.methods()  &&  that.methods()->bstore_methods )
		{
			seteof( that, 0 );
		}
	}
	
}

