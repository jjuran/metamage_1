/*
	conjoin.cc
	----------
*/

#include "vfs/filehandle/primitives/conjoin.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/methods/general_method_set.hh"


namespace vfs
{
	
	namespace p7 = poseven;
	
	
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
		
		p7::throw_errno( EINVAL );
	}
	
}
