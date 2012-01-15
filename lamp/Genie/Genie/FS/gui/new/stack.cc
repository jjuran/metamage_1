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
#include "Genie/FS/FSTreeCache.hh"
#include "Genie/FS/FSTree_Property.hh"
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
	
	
	struct Null_KeyName_Traits
	{
		typedef plus::string Key;
		
		static const plus::string& NameFromKey( const Key& key )
		{
			return key;
		}
		
		static const Key& KeyFromName( const plus::string& name )
		{
			return name;
		}
	};
	
	
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
	
	
	boost::intrusive_ptr< Ped::View > StackFactory( const FSTree* delegate )
	{
		return new Stack( delegate );
	}
	
	
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
	
	class FSTree_Stack_Subview : public FSTree_View
	{
		public:
			FSTree_Stack_Subview( const FSTreePtr&     parent,
		                          const plus::string&  name)
			:
				FSTree_View( parent, name, get_subview, NULL )
			{
			}
			
			void Delete() const;
	};
	
	void FSTree_Stack_Subview::Delete() const
	{
		FSTree_View::Delete();  // throws if nonexistent
		
		delete_subview( ParentRef().get(), Name() );
	}
	
	
	class FSTree_Stack : public FSTree
	{
		public:
			typedef ViewList  Sequence;
			
			FSTree_Stack( const FSTreePtr&     parent,
		                  const plus::string&  name)
			:
				FSTree( parent, name, S_IFDIR | 0700 )
			{
			}
			
			FSTreePtr Lookup_Child( const plus::string& name, const FSTree* parent ) const
			{
				return new FSTree_Stack_Subview( (parent ? parent : this)->Self(), name );
			}
			
			void IterateIntoCache( FSTreeCache& cache ) const;
			
			void Delete() const  { FSTree_new_stack::DestroyDelegate( this ); }
	};
	
	class Stack_IteratorConverter
	{
		public:
			FSNode operator()( const ViewList::value_type& value ) const
			{
				const ino_t inode = 0;
				
				return FSNode( inode, value.name );
			}
	};
	
	void FSTree_Stack::IterateIntoCache( FSTreeCache& cache ) const
	{
		Stack_IteratorConverter converter;
		
		const Sequence& sequence = gStack_Parameters_Map[ this ].v;
		
		std::transform( sequence.begin(),
		                sequence.end(),
		                std::back_inserter( cache ),
		                converter );
	}
	
	
	FSTreePtr FSTree_new_stack::CreateDelegate( const FSTreePtr&     parent,
	                                            const plus::string&  name ) const
	{
		return new FSTree_Stack( parent, name );
	}
	
	void FSTree_new_stack::DestroyDelegate( const FSTree* delegate )
	{
		gStack_Parameters_Map.erase( delegate );
	}
	
}

