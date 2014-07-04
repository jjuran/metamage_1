/*
	hangup.cc
	---------
*/

#include "vfs/filehandle/primitives/hangup.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/filehandle/methods/terminal_method_set.hh"


namespace vfs
{
	
	void hangup( filehandle& that )
	{
		// throws if not a terminal
		
		if ( hangup_method hangup = that.terminal_methods().hangup )
		{
			hangup( &that );
		}
	}
	
}
