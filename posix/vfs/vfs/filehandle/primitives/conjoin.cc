/*
	conjoin.cc
	----------
*/

#include "vfs/filehandle/primitives/conjoin.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/methods/general_method_set.hh"


namespace vfs
{
	
	void conjoin( filehandle& that, filehandle& target )
	{
		if ( const filehandle_method_set* methods = that.methods() )
		{
			if ( const general_method_set* general_methods = methods->general_methods )
			{
				if ( general_methods->conjoin )
				{
					general_methods->conjoin( that, target );
					
					return;
				}
			}
		}
		
		that.Attach( &target );
	}
	
}
