/*	========
 *	Views.hh
 *	========
 */

#ifndef GENIE_FILESYSTEM_VIEWS_HH
#define GENIE_FILESYSTEM_VIEWS_HH

// Standard C++
#include <memory>

// Debug
#include "debug/assert.hh"

// plus
#include "plus/var_string.hh"

// Genie
#include "Genie/FS/property.hh"
#include "Genie/FS/FSTree_Directory.hh"


struct Rect;

namespace Pedestal
{
	
	class View;
	
}

namespace Genie
{
	
	void RemoveAllViewParameters( const FSTree* parent );
	
	
	typedef boost::intrusive_ptr< Pedestal::View > (*ViewFactory)( const FSTree* delegate );
	
	
	const FSTree* GetViewWindowKey( const FSTree* view );
	
	bool InvalidateWindowForView( const FSTree* view );
	
	
	inline const FSTree* GetViewKey( const FSTree* that )  { return that->ParentRef().get(); }
	
	
	template < class Serialize, typename Serialize::result_type& (*Access)( const FSTree* ) >
	struct Const_View_Property : readonly_property
	{
		static void get( plus::var_string& result, const FSTree* that, bool binary )
		{
			typedef typename Serialize::result_type result_type;
			
			const result_type& value = Access( GetViewKey( that ) );
			
			Serialize::deconstruct::apply( result, value, binary );
		}
	};
	
	template < class Serialize, typename Serialize::result_type& (*Access)( const FSTree* ) >
	struct View_Property : readwrite_property
	{
		static void get( plus::var_string& result, const FSTree* that, bool binary )
		{
			typedef typename Serialize::result_type result_type;
			
			const result_type& value = Access( GetViewKey( that ) );
			
			Serialize::deconstruct::apply( result, value, binary );
		}
		
		static void set( const FSTree* that, const char* begin, const char* end, bool binary )
		{
			const FSTree* view = GetViewKey( that );
			
			Access( view ) = Serialize::reconstruct::apply( begin, end, binary );
			
			InvalidateWindowForView( view );
		}
	};
	
	
	class FSTree_new_View : public FSTree
	{
		private:
			typedef const FSTree_Premapped::Mapping* Mappings;
			
			typedef void (*Destructor)( const FSTree* delegate );
			
			ViewFactory  itsFactory;
			Mappings     itsMappings;
			Destructor   itsDestructor;
			
			virtual FSTreePtr CreateDelegate( const FSTreePtr&     parent,
			                                  const plus::string&  name ) const;
		
		public:
			FSTree_new_View( const FSTreePtr&     parent,
			                 const plus::string&  name,
			                 ViewFactory          factory,
			                 Mappings             mappings,
			                 Destructor           dtor )
			:
				FSTree( parent, name ),
				itsFactory( factory ),
				itsMappings( mappings ),
				itsDestructor( dtor )
			{
			}
			
			void HardLink( const FSTreePtr& target ) const;
	};
	
	typedef boost::intrusive_ptr< Pedestal::View >& (*ViewGetter)( const FSTree* );
	
	class FSTree_View : public FSTree
	{
		private:
			ViewGetter itsGetter;
			
			// Non-copyable
			FSTree_View           ( const FSTree_View& );
			FSTree_View& operator=( const FSTree_View& );
		
		public:
			FSTree_View( const FSTreePtr&     parent,
			             const plus::string&  name,
			             ViewGetter           get = NULL )
			:
				FSTree( parent, name ),
				itsGetter( get )
			{
			}
			
			const FSTree* ParentKey() const  { return ParentRef().get(); }
			
			bool IsFile() const  { return false; }
			
			bool IsDirectory() const  { return Exists(); }
			
			bool Exists() const;
			
			void SetTimes() const;
			
			void Delete() const;
			
			void CreateDirectory( mode_t mode ) const;
			
			FSTreePtr Lookup( const plus::string& name, const FSTree* parent ) const;
			
			FSIteratorPtr Iterate() const;
			
			virtual boost::intrusive_ptr< Pedestal::View >& Get() const
			{
				ASSERT( itsGetter != NULL );
				
				return itsGetter( ParentKey() );
			}
	};
	
}

#endif

