/*
	Genie/FS/sys/mac/thng.cc
	------------------------
*/

#include "Genie/FS/sys/mac/thng.hh"

// Standard C++
#include <algorithm>

// plus
#include "plus/hexidecimal.hh"
#include "plus/make_string.hh"
#include "plus/quad.hh"
#include "plus/var_string.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Nitrogen
#include "Nitrogen/Components.hh"

// Genie
#include "Genie/FS/basic_directory.hh"
#include "Genie/FS/FSTreeCache.hh"
#include "Genie/FS/FSTree_Generated.hh"
#include "Genie/FS/FSTree_IconSuite.hh"
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/Utilities/canonical_32_bit_hex.hh"


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
	
	inline nucleus::owned< IconSuiteRef > GetComponentIconSuite( Component component )
	{
		::IconSuiteRef iconSuite = NULL;
		
		ThrowOSStatus( ::GetComponentIconSuite( component, &iconSuite ) );
		
		return nucleus::owned< IconSuiteRef >::seize( IconSuiteRef( iconSuite ) );
	}
	
}

namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	static Component GetKeyFromParent( const FSTreePtr& parent )
	{
		return (Component) plus::decode_32_bit_hex( parent->Name() );
	}
	
	static Component GetKey( const FSTree* that )
	{
		return GetKeyFromParent( that->ParentRef() );
	}
	
	
	static bool is_valid_Component_name( const plus::string& name )
	{
		if ( !canonical_32_bit_hex::applies( name ) )
		{
			return false;
		}
		
		const Component component = (Component) plus::decode_32_bit_hex( name );
		
		if ( component == NULL )
		{
			return false;
		}
		
		try
		{
			(void) N::GetComponentInfo( component );
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
	
	static FSTreePtr thng_lookup( const FSTreePtr& parent, const plus::string& name )
	{
		if ( !is_valid_Component_name( name ) )
		{
			poseven::throw_errno( ENOENT );
		}
		
		return Premapped_Factory( parent, name, sys_mac_thng_REF_Mappings );
	}
	
	class thng_IteratorConverter
	{
		public:
			FSNode operator()( Component component ) const
			{
				const ino_t inode = 0;
				
				plus::string name = plus::encode_32_bit_hex( (unsigned) component );
				
				return FSNode( inode, name );
			}
	};
	
	static void thng_iterate( FSTreeCache& cache )
	{
		thng_IteratorConverter converter;
		
		N::Component_Container sequence = N::Components();
		
		std::transform( sequence.begin(),
		                sequence.end(),
		                std::back_inserter( cache ),
		                converter );
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
		static void get( plus::var_string& result, const FSTree* that, bool binary )
		{
			const Component comp = GetKey( that );
			
			const ComponentDescription desc = N::GetComponentInfo( comp );
			
			result = plus::encode_quad( Accessor::Get( desc ) );
		}
	};
	
	static plus::string string_from_stringhandle( N::Handle h )
	{
		N::Str255 result;
		
		if ( std::size_t size = N::GetHandleSize( h ) )
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
		
		return plus::make_string( result );
	}
	
	struct sys_mac_thng_REF_name
	{
		static void get( plus::var_string& result, const FSTree* that, bool binary )
		{
			const Component comp = GetKey( that );
			
			n::owned< N::Handle > name = N::NewHandle( 0 );
			
			(void) N::GetComponentInfo( comp, name );
			
			result = string_from_stringhandle( name );
		}
	};
	
	struct sys_mac_thng_REF_info
	{
		static void get( plus::var_string& result, const FSTree* that, bool binary )
		{
			const Component comp = GetKey( that );
			
			n::owned< N::Handle > info = N::NewHandle( 0 );
			
			(void) N::GetComponentInfo( comp, N::Handle(), info );
			
			result = string_from_stringhandle( info );
		}
	};
	
	// perl -e 'for ( @ARGV ) { ++$h{$_} if -f "$_/icon" } print( "$_: " ), system( "cat $_/name" ) for sort keys %h' *
	// perl -e 'for ( @ARGV ) { ++$h{$_} if -f "$_/suite" } print( "$_: " ), system( "cat $_/name" ) for sort keys %h' *
	
	struct sys_mac_thng_REF_icon
	{
		static plus::string Get( const FSTree* that )
		{
			const Component comp = GetKey( that );
			
			n::owned< N::Handle > icon = N::NewHandle( 0 );
			
			(void) N::GetComponentInfo( comp, N::Handle(), N::Handle(), icon );
			
			std::size_t size = N::GetHandleSize( icon );
			
			if ( size == 0 )
			{
				p7::throw_errno( ENOENT );
			}
			
			plus::var_string result;
			
			char* p = result.reset( size );
			
			std::memcpy( p, *icon.get().Get(), size );
			
			return result;
		}
	};
	
	static inline bool nonzero( char c )
	{
		return c;
	}
	
	static FSTreePtr IconSuite_Factory( const FSTreePtr&     parent,
	                                    const plus::string&  name,
	                                    const void*          args )
	{
		const Component comp = GetKeyFromParent( parent );
		
		n::shared< N::IconSuiteRef > iconSuite = N::GetComponentIconSuite( comp );
		
		const ::Handle h = iconSuite.get().Get();
		
		if ( h == NULL )
		{
			p7::throw_errno( ENOENT );
		}
		
		const std::size_t size = N::GetHandleSize( Handle( h ) );
		
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
	static FSTreePtr Generated_Factory( const FSTreePtr&     parent,
	                                    const plus::string&  name,
	                                    const void*          args )
	{
		return New_FSTree_Generated( parent,
		                             name,
		                             &Property::Get );
	}
	
	template < class Property >
	static FSTreePtr Property_Factory( const FSTreePtr&     parent,
	                                   const plus::string&  name,
	                                   const void*          args )
	{
		return New_FSTree_Property( parent,
		                            name,
		                            &Property::get );
	}
	
	const FSTree_Premapped::Mapping sys_mac_thng_REF_Mappings[] =
	{
		{ "type",         &Property_Factory< sys_mac_thng_REF_code< ComponentDescription_Type         > > },
		{ "subtype",      &Property_Factory< sys_mac_thng_REF_code< ComponentDescription_SubType      > > },
		{ "manufacturer", &Property_Factory< sys_mac_thng_REF_code< ComponentDescription_Manufacturer > > },
		
		{ "name",         &Property_Factory< sys_mac_thng_REF_name > },
		{ "info",         &Property_Factory< sys_mac_thng_REF_info > },
		
		{ "icon",         &Generated_Factory< sys_mac_thng_REF_icon  > },
		
		{ "suite",        &IconSuite_Factory },
		
		{ NULL, NULL }
	};
	
	FSTreePtr New_FSTree_sys_mac_thng( const FSTreePtr&     parent,
	                                   const plus::string&  name,
	                                   const void*          args )
	{
		return new_basic_directory( parent, name, thng_lookup, thng_iterate );
	}
	
}

