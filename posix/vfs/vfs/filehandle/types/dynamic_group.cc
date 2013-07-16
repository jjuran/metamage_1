/*
	dynamic_group.cc
	----------------
*/

#include "vfs/filehandle/types/dynamic_group.hh"

// Standard C
#include <errno.h>

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/filehandle.hh"


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	filehandle_ptr
	//
	get_dynamic_element_from_group_by_id( const dynamic_group&  group,
	                                      dynamic_element_id    id )
	{
		dynamic_group::const_iterator it = group.find( id );
		
		if ( it == group.end() )
		{
			p7::throw_errno( ENOENT );
		}
		
		filehandle* h = (filehandle*) it->second;
		
		return h;
	}
	
}

