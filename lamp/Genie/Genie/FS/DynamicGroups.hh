/*	================
 *	DynamicGroups.hh
 *	================
 */

#ifndef GENIE_FILESYSTEM_DYNAMICGROUPS_HH
#define GENIE_FILESYSTEM_DYNAMICGROUPS_HH

// plus
#include "plus/string.hh"

// Genie
#include "Genie/FS/FSTreePtr.hh"
#include "Genie/FS/data_method_set.hh"
#include "Genie/FS/node_method_set.hh"
#include "Genie/IO/DynamicGroup.hh"


namespace Genie
{
	
	typedef IOPtr (*DynamicElementGetter)( std::size_t );
	
	
	IOPtr get_dynamic_element_from_node( const FSTree* node, DynamicElementGetter getter );
	
	
	struct dynamic_group_extra
	{
		const DynamicGroup*     group;
		const node_method_set*  methods;
	};
	
	
	template < class Handle >
	struct dynamic_group_element
	{
		static IOPtr open( const FSTree* node, int flags, mode_t mode );
		
		static const data_method_set data_methods;
		static const node_method_set node_methods;
		
		static const dynamic_group_extra extra;
	};
	
	template < class Handle >
	const dynamic_group_extra dynamic_group_element< Handle >::extra =
	{
		&GetDynamicGroup< Handle >(),
		&dynamic_group_element< Handle >::node_methods
	};
	
	template < class Handle >
	IOPtr dynamic_group_element< Handle >::open( const FSTree* node, int flags, mode_t mode )
	{
		return get_dynamic_element_from_node( node, &GetDynamicElementByID< Handle > );
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
	
	FSTreePtr dynamic_group_factory( const FSTreePtr&     parent,
	                                 const plus::string&  name,
	                                 const void*          args );
	
}

#endif

