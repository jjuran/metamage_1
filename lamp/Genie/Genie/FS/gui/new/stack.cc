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

// Genie
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/FSTreeCache.hh"
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
		
		Named_Subview()
		{
		}
		
		Named_Subview( const plus::string& n ) : name( n )
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
	
	
	static void DestroyDelegate( const FSTree* delegate );
	
	
	static boost::intrusive_ptr< Pedestal::View >&
	//
	get_subview( const FSTree* parent, const plus::string& name )
	{
		Stack_Parameters& params = gStack_Parameters_Map[ parent ];
		
		return find_or_append_subview( params, name ).view;
	}
	
	static void delete_subview( const FSTree* parent, const plus::string& name )
	{
		Stack_Parameters& params = gStack_Parameters_Map[ parent ];
		
		Named_Subview* subview = find_subview( params, name );
		
		ASSERT( subview != NULL );
		
		ViewList& v = params.v;
		
		v.erase( v.begin() + (subview - &v[0]) );
	}
	
	
	static void stack_remove( const FSTree* node )
	{
		DestroyDelegate( node );
	}
	
	static FSTreePtr stack_lookup( const FSTree* node, const plus::string& name, const FSTree* parent )
	{
		return New_View( parent, name, get_subview, delete_subview );
	}
	
	
	class Stack_IteratorConverter
	{
		public:
			FSNode operator()( const ViewList::value_type& value ) const
			{
				const ino_t inode = 0;
				
				return FSNode( inode, value.name );
			}
	};
	
	static void stack_listdir( const FSTree* node, FSTreeCache& cache )
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
	                                                const FSTreePtr&     parent,
	                                                const plus::string&  name )
	{
		return new FSTree( parent, name, S_IFDIR | 0700, &stack_methods );
	}
	
	static void DestroyDelegate( const FSTree* delegate )
	{
		gStack_Parameters_Map.erase( delegate );
	}
	
	FSTreePtr New_stack( const FSTreePtr&     parent,
	                     const plus::string&  name,
	                     const void*          args )
	{
		return New_new_view( parent,
		                     name,
		                     &StackFactory,
		                     NULL,
		                     &DestroyDelegate,
		                     &create_delegate_for_new_stack );
	}
	
}

