/*
	append.cc
	---------
*/

#include "vfs/filehandle/primitives/append.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/filehandle/methods/bstore_method_set.hh"
#include "vfs/filehandle/primitives/geteof.hh"
#include "vfs/filehandle/primitives/pwrite.hh"


namespace vfs
{
	
	ssize_t append( filehandle& that, const char* buffer, size_t n )
	{
		const vfs::bstore_method_set& bstore_methods = that.bstore_methods();
		
		if ( bstore_methods.append == 0 )  // NULL
		{
			const off_t eof_mark = that.set_mark( geteof( that ) );
			
			return pwrite( that, buffer, n, eof_mark );
		}
		
		return bstore_methods.append( &that, buffer, n );
	}
	
}
