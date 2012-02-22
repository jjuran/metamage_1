/*	================
 *	DynamicGroups.hh
 *	================
 */

#ifndef GENIE_FILESYSTEM_DYNAMICGROUPS_HH
#define GENIE_FILESYSTEM_DYNAMICGROUPS_HH

// POSIX
#include <sys/stat.h>

// Genie 
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/data_method_set.hh"
#include "Genie/FS/node_method_set.hh"
#include "Genie/IO/DynamicGroup.hh"


namespace Genie
{
	
	typedef IOPtr (*DynamicElementGetter)( std::size_t );
	
	
	IOPtr get_dynamic_element_from_node( const FSTree* node, DynamicElementGetter getter );
	
	
	class FSTree_DynamicGroup_Base : public FSTree
	{
		public:
			typedef DynamicGroup Sequence;
			
			FSTree_DynamicGroup_Base( const FSTreePtr&     parent,
			                          const plus::string&  name )
			:
				FSTree( parent, name, S_IFDIR | 0700 )
			{
			}
			
			virtual const Sequence& ItemSequence() const = 0;
			
			virtual DynamicElementGetter Getter() const = 0;
			
			virtual const node_method_set* node_methods() const = 0;
			
			FSTreePtr Lookup_Child( const plus::string& name, const FSTree* parent ) const;
			
			void IterateIntoCache( FSTreeCache& cache ) const;
	};
	
	
	template < class Handle >
	struct dynamic_group_element
	{
		static IOPtr open( const FSTree* node, int flags, mode_t mode );
		
		static const data_method_set data_methods;
		static const node_method_set node_methods;
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
	
	template < class Handle >
	class FSTree_DynamicGroup : public FSTree_DynamicGroup_Base
	{
		public:
			FSTree_DynamicGroup( const FSTreePtr&     parent,
			                     const plus::string&  name )
			:
				FSTree_DynamicGroup_Base( parent, name )
			{
			}
			
			const Sequence& ItemSequence() const
			{
				return GetDynamicGroup< Handle >();
			}
			
			DynamicElementGetter Getter() const
			{
				return &GetDynamicElementByID< Handle >;
			}
			
			const node_method_set* node_methods() const
			{
				return &dynamic_group_element< Handle >::node_methods;
			}
	};
	
}

#endif

