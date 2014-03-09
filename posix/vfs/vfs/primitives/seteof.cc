/*
	seteof.cc
	---------
*/

#include "vfs/primitives/seteof.hh"

// POSIX
#include <fcntl.h>
#include <sys/stat.h>

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/node.hh"
#include "vfs/methods/data_method_set.hh"
#include "vfs/methods/node_method_set.hh"
#include "vfs/filehandle/primitives/seteof.hh"


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	void seteof( const node& that, off_t length )
	{
		const node_method_set* methods = that.methods();
		
		const data_method_set* data_methods;
		
		if ( methods  &&  (data_methods = methods->data_methods) )
		{
			if ( data_methods->seteof )
			{
				data_methods->seteof( &that, length );
				
				return;
			}
			
			if ( data_methods->open )
			{
				filehandle_ptr h = data_methods->open( &that, O_WRONLY, 0 );
				
				seteof( *h, length );
				
				return;
			}
		}
		
		// This confuses MWCPPC when optimizing:
		//p7::throw_errno( S_ISDIR( that.filemode() ) ? EISDIR : EINVAL );
		// internal compiler error: File: 'PCodeUtilities.c' Line: 80
		
		const volatile int error = S_ISDIR( that.filemode() ) ? EISDIR : EINVAL;
		
		p7::throw_errno( error );
	}
	
}

