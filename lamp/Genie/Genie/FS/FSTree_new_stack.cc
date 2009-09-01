/*	===================
 *	FSTree_new_stack.cc
 *	===================
 */

#include "Genie/FS/FSTree_new_stack.hh"

// Standard C++
#include <vector>

// Nucleus
#include "Nucleus/NAssert.h"

// Pedestal
#include "Pedestal/Stack.hh"

// Genie
#include "Genie/FS/FSTree_Property.hh"


/*
	cd /new/window
	ln /new/stack view
	ln /new/color view/background
	ln /new/frame view
	ls view
		frame
		background
	
*/

namespace Genie
{
	
	namespace Ped = Pedestal;
	
	
	struct Null_KeyName_Traits
	{
		typedef std::string Key;
		
		static const std::string& NameFromKey( const Key& key )
		{
			return key;
		}
		
		static const Key& KeyFromName( const std::string& name )
		{
			return name;
		}
	};
	
	
	struct Named_Subview
	{
		std::string                     name;
		boost::shared_ptr< Ped::View >  view;
		
		Named_Subview()
		{
		}
		
		Named_Subview( const std::string& n ) : name( n )
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
	find_subview( Stack_Parameters& params, const std::string& name )
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
	find_or_append_subview( Stack_Parameters& params, const std::string& name )
	{
		ViewList& v = params.v;
		
		if ( Named_Subview* subview = find_subview( params, name ) )
		{
			return *subview;
		}
		
		v.push_back( Named_Subview( name ) );
		
		return v.back();
	}
	
	typedef std::map< const FSTree*, Stack_Parameters > Stack_Parameters_Map;
	
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
	
	
	boost::shared_ptr< Ped::View > StackFactory( const FSTree* delegate )
	{
		return boost::shared_ptr< Ped::View >( new Stack( delegate ) );
	}
	
	
	class FSTree_Stack_Subview : public FSTree_View
	{
		public:
			FSTree_Stack_Subview( const FSTreePtr&    parent,
		                          const std::string&  name)
			:
				FSTree_View( parent, name )
			{
			}
			
			void Delete() const;
			
			boost::shared_ptr< Pedestal::View >& Get() const
			{
				Stack_Parameters& params = gStack_Parameters_Map[ ParentRef().get() ];
				
				return find_or_append_subview( params, Name() ).view;
			}
	};
	
	void FSTree_Stack_Subview::Delete() const
	{
		FSTree_View::Delete();  // throws if nonexistent
		
		Stack_Parameters& params = gStack_Parameters_Map[ ParentRef().get() ];
		
		Named_Subview* subview = find_subview( params, Name() );
		
		ASSERT( subview != NULL );
		
		ViewList& v = params.v;
		
		v.erase( v.begin() + (subview - &v[0]) );
	}
	
	
	struct Stack_Details : Null_KeyName_Traits
	{
		typedef ViewList  Sequence;
		
		const FSTree* itsSelfKey;
		
		Stack_Details() : itsSelfKey()
		{
		}
		
		Stack_Details( const FSTree* self ) : itsSelfKey( self )
		{
		}
		
		const Sequence& ItemSequence() const  { return gStack_Parameters_Map[ itsSelfKey ].v; }
		
		const std::string& KeyFromValue( const Sequence::value_type& value ) const  { return value.name; }
		
		bool KeyIsValid( const Key& key ) const  { return true; }
		
		FSTreePtr GetChildNode( const FSTreePtr&    parent,
		                        const std::string&  name,
		                        const Key&          key ) const;
	};
	
	FSTreePtr Stack_Details::GetChildNode( const FSTreePtr&    parent,
		                                   const std::string&  name,
		                                   const Key&          key ) const
	{
		return FSTreePtr( new FSTree_Stack_Subview( parent, name ) );
	}
	
	class FSTree_Stack : public FSTree_Sequence< Stack_Details >
	{
		public:
			FSTree_Stack( const FSTreePtr&    parent,
		                  const std::string&  name)
			:
				FSTree_Sequence< Stack_Details >( parent, name, this )
			{
			}
			
			void Delete() const  { FSTree_new_stack::DestroyDelegate( this ); }
	};
	
	
	FSTreePtr FSTree_new_stack::CreateDelegate( const FSTreePtr&    parent,
	                                            const std::string&  name ) const
	{
		FSTreePtr delegate( new FSTree_Stack( parent, name ) );
		
		return delegate;
	}
	
	void FSTree_new_stack::DestroyDelegate( const FSTree* delegate )
	{
		gStack_Parameters_Map.erase( delegate );
	}
	
}

