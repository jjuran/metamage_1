/*
	conjoin.cc
	----------
*/

#include "vfs/filehandle/primitives/conjoin.hh"

// vfs
#include "vfs/filehandle.hh"


namespace vfs
{
	
	void conjoin( filehandle& that, filehandle& target )
	{
		that.Attach( &target );
	}
	
}
