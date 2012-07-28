/*
	gui/new/stack.cc
	----------------
*/

#include "Genie/FS/gui/new/stack.hh"

// Standard C++
#include <vector>

// POSIX
#include <sys/stat.h>

// Debug
#include "debug/assert.hh"

// Pedestal
#include "Pedestal/Stack.hh"

// vfs
#include "vfs/dir_contents.hh"
#include "vfs/dir_entry.hh"
#include "vfs/primitives/remove.hh"

// Genie
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/Views.hh"
#include "Genie/FS/dir_method_set.hh"
#include "Genie/FS/node_method_set.hh"
#include "Genie/Utilities/simple_map.hh"


/*
	cd /gui/new/port
	ln /gui/new/stack view
	ln /gui/new/color view/background
	ln /gui/new/frame view
	ls view
		frame
		background
	
*/

namespace Genie
{
	
	namespace Ped = Pedestal;
	
	
	struct Named_Subview
	{
		plus::string                       name;
		boost::intrusive_ptr< Ped::View >  view;
		const vfs::node*                   node;
		
		Named_Subview() : node()
		{
		}
		
		Named_Subview( const plus::string& n ) : name( n ), node()
		{
		}
	};
	
	typedef std::vector< Named_Subview > ViewList;
	
	struct Stack_Parameters
	{
		ViewList  v;
		
		Stack_Parameters()
		{
		}
	};
	
	static Named_Subview*
	//
	find_subview( Stack_Parameters& params, const plus::string& name )
	{
		ViewList& v = params.v;
		
		for ( ViewList::iterator it = v.begin();  it != v.end();  ++it )
		{
			if ( it->name == name )
			{
				return &*it;
			}
		}
		
		return NULL;
	}
	
	static Named_Subview&
	//
	find_or_append_subview( Stack_Parameters& params, const plus::string& name )
	{
		ViewList& v = params.v;
		
		if ( Named_Subview* subview = find_subview( params, name ) )
		{
			return *subview;
		}
		
		v.push_back( Named_Subview( name ) );
		
		return v.back();
	}
	
	typedef simple_map< const FSTree*, Stack_Parameters > Stack_Parameters_Map;
	
	static Stack_Parameters_Map gStack_Parameters_Map;
	
	
	class Stack : public Ped::Stack
	{
		private:
			typedef const FSTree* Key;
			
			Key itsKey;
		
		public:
			Stack( Key key ) : itsKey( key )
			{
			}
			
			unsigned ViewCount() const  { return gStack_Parameters_Map[ itsKey ].v.size(); }
			
			View& GetNthView( unsigned n )  { return *gStack_Parameters_Map[ itsKey ].v.at( n - 1 ).view; }
	};
	
	
	static boost::intrusive_ptr< Ped::View > StackFactory( const FSTree* delegate )
	{
		return new Stack( delegate );
	}
	
	
	static boost::intrusive_ptr< Pedestal::View >&
	//
	get_subview( const FSTree* layer, const plus::string& name )
	{
		Stack_Parameters& params = gStack_Parameters_Map[ layer->owner() ];
		
		return find_or_append_subview( params, layer->name() ).view;
	}
	
	static void destroy_layer( const FSTree* that )
	{
		Stack_Parameters& params = gStack_Parameters_Map[ that->owner() ];
		
		Named_Subview* subview = find_subview( params, that->name() );
		
		ASSERT( subview != NULL );
		
		ViewList& v = params.v;
		
		v.erase( v.begin() + (subview - &v[0]) );
	}
	
	
	static vfs::node_ptr stack_subview_factory( const vfs::node*     parent,
	                                            const plus::string&  name,
	                                            const void*          args )
	{
		return New_View( parent, name, get_subview, NULL );
	}
	
	static const vfs::fixed_mapping stack_mappings[] =
	{
		{ "v", &stack_subview_factory },
		
		{ NULL, NULL }
	};
	
	
	static void stack_remove( const FSTree* node )
	{
		Stack_Parameters& params = gStack_Parameters_Map[ node ];
		
		while ( !params.v.empty() )
		{
			remove( params.v.back().node );
		}
		
		gStack_Parameters_Map.erase( node );
	}
	
	static FSTreePtr stack_lookup( const FSTree* node, const plus::string& name, const FSTree* parent )
	{
		Named_Subview& layer = find_or_append_subview( gStack_Parameters_Map[ node ], name );
		
		if ( layer.node == NULL )
		{
			vfs::node_ptr layer_node = vfs::fixed_dir( parent,
			                                           name,
			                                           stack_mappings,
			                                           &destroy_layer );
			
			layer.node = layer_node.get();
			
			return layer_node;
		}
		
		return layer.node;
	}
	
	
	class Stack_IteratorConverter
	{
		public:
			vfs::dir_entry operator()( const ViewList::value_type& value ) const
			{
				const ino_t inode = 0;
				
				return vfs::dir_entry( inode, value.name );
			}
	};
	
	static void stack_listdir( const FSTree* node, vfs::dir_contents& cache )
	{
		typedef ViewList Sequence;
		
		Stack_IteratorConverter converter;
		
		const Sequence& sequence = gStack_Parameters_Map[ node ].v;
		
		std::transform( sequence.begin(),
		                sequence.end(),
		                std::back_inserter( cache ),
		                converter );
	}
	
	static const dir_method_set stack_dir_methods =
	{
		&stack_lookup,
		&stack_listdir
	};
	
	static const node_method_set stack_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		&stack_remove,
		NULL,
		NULL,
		NULL,
		&stack_dir_methods
	};
	
	
	static FSTreePtr create_delegate_for_new_stack( const FSTree*        node,
	                                                const FSTree*        parent,
	                                                const plus::string&  name )
	{
		return new FSTree( parent, name, S_IFDIR | 0700, &stack_methods );
	}
	
	FSTreePtr New_stack( const FSTree*        parent,
	                     const plus::string&  name,
	                     const void*          args )
	{
		return New_new_view( parent,
		                     name,
		                     &StackFactory,
		                     NULL,
		                     &stack_remove,
		                     &create_delegate_for_new_stack );
	}
	
}

