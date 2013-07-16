/*
	dynamic_group.hh
	----------------
*/

#ifndef VFS_NODE_TYPES_DYNAMICGROUP_HH
#define VFS_NODE_TYPES_DYNAMICGROUP_HH

// plus
#include "plus/string.hh"

// vfs
#include "vfs/node_ptr.hh"
#include "vfs/filehandle/types/dynamic_group.hh"
#include "vfs/methods/data_method_set.hh"
#include "vfs/methods/node_method_set.hh"


namespace vfs
{
	
	typedef filehandle_ptr (*dynamic_element_getter)( std::size_t );
	
	
	filehandle_ptr get_dynamic_element_from_node( const node*             that,
	                                              dynamic_element_getter  getter );
	
	
	struct dynamic_group_extra
	{
		const dynamic_group*    group;
		const node_method_set*  methods;
	};
	
	
	template < class Handle >
	struct dynamic_group_element
	{
		static filehandle_ptr open( const node* that, int flags, mode_t mode );
		
		static const data_method_set data_methods;
		static const node_method_set node_methods;
		
		static const dynamic_group_extra extra;
	};
	
	template < class Handle >
	const dynamic_group_extra dynamic_group_element< Handle >::extra =
	{
		&get_dynamic_group< Handle >(),
		&dynamic_group_element< Handle >::node_methods
	};
	
	template < class Handle >
	filehandle_ptr dynamic_group_element< Handle >::open( const node* that, int flags, mode_t mode )
	{
		return get_dynamic_element_from_node( that, &get_dynamic_element_by_id< Handle > );
	}
	
	template < class Handle >
	const data_method_set dynamic_group_element< Handle >::data_methods =
	{
		&dynamic_group_element< Handle >::open
	};
	
	template < class Handle >
	const node_method_set dynamic_group_element< Handle >::node_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		&data_methods
	};
	
	node_ptr dynamic_group_factory( const node*          parent,
	                                const plus::string&  name,
	                                const void*          args );
	
}

#endif

