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
	
	template < class Handle >
	dynamic_group& get_dynamic_group()
	{
		static dynamic_group the_group;
		
		return the_group;
	}
	
	filehandle_ptr
	//
	get_dynamic_element_from_group_by_id( const dynamic_group&  group,
	                                      dynamic_element_id    id );
	
	template < class Handle >
	filehandle_ptr get_dynamic_element_by_id( dynamic_element_id id )
	{
		dynamic_group& group( get_dynamic_group< Handle >() );
		
		return get_dynamic_element_from_group_by_id( group, id );
	}
	
	inline
	void set_dynamic_element_of_group_by_id( dynamic_group&      group,
	                                         dynamic_element_id  id,
	                                         const filehandle*   h )
	{
		group[ id ] = h;
	}
	
	template < class Handle >
	void set_dynamic_element_by_id( dynamic_element_id id, const filehandle* h )
	{
		dynamic_group& group( get_dynamic_group< Handle >() );
		
		set_dynamic_element_of_group_by_id( group, id, h );
	}
	
}

#endif
