/*
	FSSpec_from_node.cc
	-------------------
*/

#include "MacVFS/util/FSSpec_from_node.hh"

// Standard C
#include <errno.h>
#include <sys/stat.h>

// mac-relix-utils
#include "mac_relix/FSSpec_from_stat.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/primitives/stat.hh"


namespace vfs
{
	
	namespace p7 = poseven;
	
	using mac::types::FSSpec;
	
	
	FSSpec FSSpec_from_node( const node& file )
	{
		using mac::relix::FSSpec_from_stat;
		
		struct stat stat_buffer = { 0 };
		
		try
		{
			stat( file, stat_buffer );
		}
		catch ( const p7::errno_t& err )
		{
			if ( err != ENOENT )
			{
				throw;
			}
		}
		
		FSSpec spec;
		
		p7::throw_posix_result( FSSpec_from_stat( stat_buffer, spec ) );
		
		return spec;
	}
	
}
