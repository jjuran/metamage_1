/*	======================
 *	FSTree_sys_mac_thng.cc
 *	======================
 */

#include "Genie/FS/FSTree_sys_mac_thng.hh"

// Standard C++
#include <algorithm>

// poseven
#include "poseven/types/errno_t.hh"

// Nitrogen
#include "Nitrogen/Components.h"

// Genie
#include "Genie/FS/FSSpec.hh"
#include "Genie/FS/FSTree_Directory.hh"
#include "Genie/FS/FSTree_Generated.hh"
#include "Genie/FS/FSTree_IconSuite.hh"
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/Name_OSType.hh"


namespace Nitrogen
{
	
	const ComponentDescription& AnyComponentDescription();
	
	inline Component FindNextComponent( Component                    component   = NULL,
	                                    const ComponentDescription&  description = AnyComponentDescription() )
	{
		ComponentDescription desc_copy = description;
		
		return ::FindNextComponent( component, &desc_copy );
	}
	
	
	class Component_Container
	{
		private:
			// not implemented:
			Component_Container& operator=( const Component_Container& );
		
		public:
			typedef UInt16 size_type;
			typedef SInt16 difference_type;
			
			typedef Component value_type;
			
			typedef const value_type &reference;
			typedef const value_type &const_reference;
			
			typedef const value_type *pointer;
			typedef const value_type *const_pointer;
			
			class const_iterator
			{
				friend class Component_Container;
				
				public:
					typedef Component_Container::size_type  size_type;
					typedef Component_Container::reference  reference;
					typedef Component_Container::pointer    pointer;
					
					typedef std::forward_iterator_tag iterator_category;
					
				private:
					Component component;
					
					const_reference GetReference() const
					{
						return component;
					}
					
					void Advance()
					{
						component = FindNextComponent( component );
					}
					
					const_iterator( Component c = NULL ) : component( c )
					{
					}
					
				public:
					const_iterator& operator++()      { Advance();  return *this; }
					const_iterator operator++(int)    { const_iterator old = *this; operator++(); return old; }
					
					reference operator*() const       { return GetReference(); }
					pointer operator->() const        { return &GetReference(); }
					
					friend bool operator==( const const_iterator& a, const const_iterator& b )    { return a.component == b.component; }
					friend bool operator!=( const const_iterator& a, const const_iterator& b )    { return !( a == b ); }
			};
			
			const_iterator begin() const              { return const_iterator( FindNextComponent() ); }
			const_iterator end() const                { return const_iterator(); }
	};
	
	inline Component_Container Components()
	{
		return Component_Container();
	}
	
	
	static ComponentDescription gAnyComponentDescription = { kAnyComponentType,
	                                                         kAnyComponentSubType,
	                                                         kAnyComponentManufacturer,
	                                                         0,
	                                                         kAnyComponentFlagsMask };
	
	const ComponentDescription& AnyComponentDescription()
	{
		return gAnyComponentDescription;
	}
	
	static ComponentDescription GetComponentInfo( Component  component,
	                                              Handle     name = Handle(),
	                                              Handle     info = Handle(),
	                                              Handle     icon = Handle() )
	{
		ComponentDescription result;
		
		ThrowOSStatus( ::GetComponentInfo( component,
		                                   &result,
		                                   name,
		                                   info,
		                                   icon ) );
		
		return result;
	}
	
	inline Nucleus::Owned< IconSuiteRef > GetComponentIconSuite( Component component )
	{
		::IconSuiteRef iconSuite = NULL;
		
		ThrowOSStatus( ::GetComponentIconSuite( component, &iconSuite ) );
		
		return Nucleus::Owned< IconSuiteRef >::Seize( IconSuiteRef( iconSuite ) );
	}
	
}

namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace p7 = poseven;
	
	
	struct sys_mac_thng_Details : public Pointer_KeyName_Traits< Component >
	{
		typedef Nitrogen::Component_Container Sequence;
		
		static Sequence ItemSequence()  { return Nitrogen::Components(); }
		
		static Key KeyFromValue( const Sequence::value_type& value )  { return value; }
		
		static bool KeyIsValid( const Key& key );
		
		static FSTreePtr GetChildNode( const FSTreePtr&    parent,
		                               const std::string&  name,
		                               const Key&          key );
	};
	
	typedef FSTree_Sequence< sys_mac_thng_Details > FSTree_sys_mac_thng;
	
	
	static Component GetKeyFromParent( const FSTreePtr& parent )
	{
		return Pointer_KeyName_Traits< Component >::KeyFromName( parent->Name() );
	}
	
	static Component GetKey( const FSTree* that )
	{
		return GetKeyFromParent( that->ParentRef() );
	}
	
	
	bool sys_mac_thng_Details::KeyIsValid( const Key& key )
	{
		if ( key == NULL )
		{
			return false;
		}
		
		try
		{
			(void) N::GetComponentInfo( key );
		}
		catch ( const N::OSStatus& err )
		{
			if ( err != badComponentSelector )
			{
				throw;
			}
			
			return false;
		}
		
		return true;
	}
	
	extern const FSTree_Premapped::Mapping sys_mac_thng_REF_Mappings[];
	
	FSTreePtr sys_mac_thng_Details::GetChildNode( const FSTreePtr&    parent,
		                                          const std::string&  name,
		                                          const Key&          key )
	{
		return Premapped_Factory< sys_mac_thng_REF_Mappings >( parent, name );
	}
	
	struct ComponentDescription_Type
	{
		static ::OSType Get( const ComponentDescription& description )
		{
			return description.componentType;
		}
	};
	
	struct ComponentDescription_SubType
	{
		static ::OSType Get( const ComponentDescription& description )
		{
			return description.componentSubType;
		}
	};
	
	struct ComponentDescription_Manufacturer
	{
		static ::OSType Get( const ComponentDescription& description )
		{
			return description.componentManufacturer;
		}
	};
	
	template < class Accessor >
	struct sys_mac_thng_REF_code
	{
		static std::string Get( const FSTree* that, bool binary )
		{
			const Component comp = GetKey( that );
			
			const ComponentDescription desc = N::GetComponentInfo( comp );
			
			const N::OSType code = N::OSType( Accessor::Get( desc ) );
			
			return NN::Convert< std::string >( code );
		}
	};
	
	static std::string string_from_stringhandle( N::Handle h )
	{
		N::Str255 result;
		
		if ( N::Size size = N::GetHandleSize( h ) )
		{
			unsigned const char* str = (StringPtr) *h.Get();
			
			if ( 1 + str[0] > size )
			{
				// corruption; shouldn't happen
				result[0] = size - 1;
				
				std::copy( &str[1], &str[ size ], &result[1] );
			}
			else
			{
				result = str;
			}
		}
		
		return NN::Convert< std::string >( result );
	}
	
	struct sys_mac_thng_REF_name
	{
		static std::string Get( const FSTree* that, bool binary )
		{
			const Component comp = GetKey( that );
			
			NN::Owned< N::Handle > name = N::NewHandle( 0 );
			
			(void) N::GetComponentInfo( comp, name );
			
			return string_from_stringhandle( name );
		}
	};
	
	struct sys_mac_thng_REF_info
	{
		static std::string Get( const FSTree* that, bool binary )
		{
			const Component comp = GetKey( that );
			
			NN::Owned< N::Handle > info = N::NewHandle( 0 );
			
			(void) N::GetComponentInfo( comp, N::Handle(), info );
			
			return string_from_stringhandle( info );
		}
	};
	
	// perl -e 'for ( @ARGV ) { ++$h{$_} if -f "$_/icon" } print( "$_: " ), system( "cat $_/name" ) for sort keys %h' *
	// perl -e 'for ( @ARGV ) { ++$h{$_} if -f "$_/suite" } print( "$_: " ), system( "cat $_/name" ) for sort keys %h' *
	
	struct sys_mac_thng_REF_icon
	{
		static std::string Get( const FSTree* that )
		{
			const Component comp = GetKey( that );
			
			NN::Owned< N::Handle > icon = N::NewHandle( 0 );
			
			(void) N::GetComponentInfo( comp, N::Handle(), N::Handle(), icon );
			
			N::Size size = N::GetHandleSize( icon );
			
			if ( size == 0 )
			{
				p7::throw_errno( ENOENT );
			}
			
			std::string result;
			
			result.resize( size );
			
			std::memcpy( &result[0], *icon.Get().Get(), size );
			
			return result;
		}
	};
	
	static inline bool nonzero( char c )
	{
		return c;
	}
	
	static FSTreePtr IconSuite_Factory( const FSTreePtr&    parent,
	                                    const std::string&  name )
	{
		const Component comp = GetKeyFromParent( parent );
		
		NN::Shared< N::IconSuiteRef > iconSuite = N::GetComponentIconSuite( comp );
		
		const ::Handle h = iconSuite.Get().Get();
		
		if ( h == NULL )
		{
			p7::throw_errno( ENOENT );
		}
		
		const N::Size size = N::GetHandleSize( Handle( h ) );
		
		const char* begin = *h;
		const char* end   = begin + size;
		
		if ( std::find_if( begin, end, std::ptr_fun( nonzero ) ) == end )
		{
			// IconSuiteRef handle data is all zero.
			p7::throw_errno( ENOENT );
		}
		
		return New_FSTree_IconSuite( parent, name, iconSuite );
	}
	
	template < class Property >
	static FSTreePtr Generated_Factory( const FSTreePtr&    parent,
	                                    const std::string&  name )
	{
		return New_FSTree_Generated( parent,
		                             name,
		                             &Property::Get );
	}
	
	template < class Property >
	static FSTreePtr Property_Factory( const FSTreePtr&    parent,
	                                   const std::string&  name )
	{
		return New_FSTree_Property( parent,
		                            name,
		                            &Property::Get );
	}
	
	const FSTree_Premapped::Mapping sys_mac_thng_REF_Mappings[] =
	{
		{ "type",         &Property_Factory< sys_mac_thng_REF_code< ComponentDescription_Type         > > },
		{ "subtype",      &Property_Factory< sys_mac_thng_REF_code< ComponentDescription_SubType      > > },
		{ "manufacturer", &Property_Factory< sys_mac_thng_REF_code< ComponentDescription_Manufacturer > > },
		
		{ "name",         &Property_Factory< sys_mac_thng_REF_name > },
		{ "info",         &Property_Factory< sys_mac_thng_REF_info >, true },
		
		{ "icon",         &Generated_Factory< sys_mac_thng_REF_icon  >, true },
		
		{ "suite",        &IconSuite_Factory, true },
		
		{ NULL, NULL }
	};
	
	FSTreePtr New_FSTree_sys_mac_thng( const FSTreePtr& parent, const std::string& name )
	{
		return FSTreePtr( new FSTree_sys_mac_thng( parent, name ) );
	}
	
}

