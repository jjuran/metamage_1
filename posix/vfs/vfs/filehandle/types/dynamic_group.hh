/*
	dynamic_group.hh
	----------------
*/

#ifndef VFS_FILEHANDLE_TYPES_DYNAMICGROUP_HH
#define VFS_FILEHANDLE_TYPES_DYNAMICGROUP_HH

// Standard C++
#include <map>

// vfs
#include "vfs/filehandle_ptr.hh"


namespace vfs
{
	
	typedef std::size_t dynamic_element_id;
	
	typedef std::map< dynamic_element_id, const void* > dynamic_group;
	
	filehandle_ptr
	//
	get_dynamic_element_from_group_by_id( const dynamic_group&  group,
	                                      dynamic_element_id    id );
	
	inline
	void set_dynamic_element_of_group_by_id( dynamic_group&      group,
	                                         dynamic_element_id  id,
	                                         const filehandle*   h )
	{
		group[ id ] = h;
	}
	
}

#endif
