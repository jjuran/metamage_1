/*
	get_file.cc
	----------
*/

#include "vfs/filehandle/primitives/get_file.hh"

// POSIX
#include <fcntl.h>
#include <sys/stat.h>

// gear
#include "gear/hexidecimal.hh"

// plus
#include "plus/var_string.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/node.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/methods/general_method_set.hh"
#include "vfs/node/types/anonymous.hh"


#ifndef O_EXEC
#define O_EXEC  0
#endif

#define STRLEN( s )  (sizeof "" s - 1)


namespace vfs
{
	
	static plus::string anon_name( const void* address, bool is_pipe )
	{
		plus::var_string name = is_pipe ? "pipe" : "socket";
		
		const size_t hex_offset = name.size() + STRLEN( ":[" );
		
		name += ":[12345678]";
		
		gear::encode_32_bit_hex( (long) address, &name[ hex_offset ] );
		
		return name.move();
	}
	
	static inline mode_t permmode_from_openflags( int flags )
	{
		// Not portable, but works in MacRelix
		const bool reading = flags & O_RDONLY;
		const bool writing = flags & O_WRONLY;
		const bool execing = flags & O_EXEC;
		
		return reading * 0400 | writing * 0200 | execing * 0100;
	}
	
	
	node_ptr get_file( filehandle& that )
	{
		if ( const filehandle_method_set* methods = that.methods() )
		{
			if ( const general_method_set* general_methods = methods->general_methods )
			{
				if ( general_methods->getfile )
				{
					return general_methods->getfile( &that );
				}
			}
		}
		
		if ( const node* file = that.get_file_ptr() )
		{
			return file;
		}
		
		const bool is_pipe = !(that.methods()  &&  that.methods()->socket_methods);
		
		const mode_t mode = S_IFIFO | permmode_from_openflags( that.get_flags() );
		
		return new_anonymous_node( anon_name( &that, is_pipe ), mode, &that );
	}
	
}
