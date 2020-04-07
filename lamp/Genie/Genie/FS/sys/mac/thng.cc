/*
	Genie/FS/sys/mac/thng.cc
	------------------------
*/

#include "Genie/FS/sys/mac/thng.hh"

// Standard C++
#include <algorithm>

// mac-sys-utils
#include "mac_sys/gestalt.hh"

// plus
#include "plus/hexadecimal.hh"
#include "plus/quad.hh"
#include "plus/var_string.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Nitrogen
#ifndef MAC_TOOLBOX_TYPES_OSSTATUS_HH
#include "Mac/Toolbox/Types/OSStatus.hh"
#endif

#include "Nitrogen/Components.hh"

// vfs
#include "vfs/dir_contents.hh"
#include "vfs/dir_entry.hh"
#include "vfs/node.hh"
#include "vfs/property.hh"
#include "vfs/node/types/basic_directory.hh"
#include "vfs/node/types/fixed_dir.hh"
#include "vfs/node/types/generated_file.hh"
#include "vfs/node/types/property_file.hh"

// relix-kernel
#include "relix/config/iconsuites.hh"

// Genie
#include "Genie/FS/FSTree_IconSuite.hh"
#include "Genie/Utilities/canonical_32_bit_hex.hh"


namespace Nitrogen
{
	
	using Mac::ThrowOSStatus;
	
	
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
		
		Mac::ThrowOSStatus( ::GetComponentInfo( component,
		                                        &result,
		                                        name,
		                                        info,
		                                        icon ) );
		
		return result;
	}
	
	inline nucleus::owned< IconSuiteRef > GetComponentIconSuite( Component component )
	{
		::IconSuiteRef iconSuite = NULL;
		
		Mac::ThrowOSStatus( ::GetComponentIconSuite( component, &iconSuite ) );
		
		return nucleus::owned< IconSuiteRef >::seize( IconSuiteRef( iconSuite ) );
	}
	
}

namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	static Component GetKeyFromParent( const vfs::node* parent )
	{
		return (Component) plus::decode_32_bit_hex( parent->name() );
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
		catch ( const Mac::OSStatus& err )
		{
			if ( err != badComponentSelector )
			{
				throw;
			}
			
			return false;
		}
		
		return true;
	}
	
	extern const vfs::fixed_mapping sys_mac_thng_REF_Mappings[];
	
	static vfs::node_ptr thng_lookup( const vfs::node* parent, const plus::string& name )
	{
		if ( !is_valid_Component_name( name ) )
		{
			poseven::throw_errno( ENOENT );
		}
		
		return fixed_dir( parent, name, sys_mac_thng_REF_Mappings );
	}
	
	static void thng_iterate( const vfs::node* parent, vfs::dir_contents& cache )
	{
		N::Component_Container sequence = N::Components();
		
		typedef N::Component_Container::const_iterator Iter;
		
		const Iter end = sequence.end();
		
		for ( Iter it = sequence.begin();  it != end;  ++it )
		{
			const ino_t inode = 0;
			
			plus::string name = plus::encode_32_bit_hex( (unsigned) *it );
			
			cache.push_back( vfs::dir_entry( inode, name ) );
		}
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
	
	static
	void sys_mac_thng_REF_code_get( plus::var_string& result, const vfs::node* that, bool binary, const plus::string& name )
	{
		const Component comp = GetKeyFromParent( that );
		
		const ComponentDescription desc = N::GetComponentInfo( comp );
		
		const char c = name[ 0 ];
		
		::OSType code = c == 't' ? desc.componentType
		              : c == 's' ? desc.componentSubType
		              : c == 'm' ? desc.componentManufacturer
		              :            0;
		
		result = plus::encode_quad( code );
	}
	
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
		
		return plus::string( result );
	}
	
	static
	void sys_mac_thng_REF_name_get( plus::var_string& result, const vfs::node* that, bool binary )
	{
		const Component comp = GetKeyFromParent( that );
		
		n::owned< N::Handle > name = N::NewHandle( 0 );
		
		(void) N::GetComponentInfo( comp, name );
		
		result = string_from_stringhandle( name );
	}
	
	static
	void sys_mac_thng_REF_info_get( plus::var_string& result, const vfs::node* that, bool binary )
	{
		const Component comp = GetKeyFromParent( that );
		
		n::owned< N::Handle > info = N::NewHandle( 0 );
		
		(void) N::GetComponentInfo( comp, N::Handle(), info );
		
		result = string_from_stringhandle( info );
	}
	
	// perl -e 'for ( @ARGV ) { ++$h{$_} if -f "$_/icon" } print( "$_: " ), system( "cat $_/name" ) for sort keys %h' *
	// perl -e 'for ( @ARGV ) { ++$h{$_} if -f "$_/suite" } print( "$_: " ), system( "cat $_/name" ) for sort keys %h' *
	
	struct sys_mac_thng_REF_icon
	{
		static plus::string Get( const vfs::node* parent, const plus::string& name )
		{
			const Component comp = GetKeyFromParent( parent );
			
			n::owned< N::Handle > icon = N::NewHandle( 0 );
			
			(void) N::GetComponentInfo( comp, N::Handle(), N::Handle(), icon );
			
			std::size_t size = N::GetHandleSize( icon );
			
			if ( size == 0 )
			{
				p7::throw_errno( ENOENT );
			}
			
			plus::string result;
			
			char* p = result.reset( size );
			
			std::memcpy( p, *icon.get().Get(), size );
			
			return result;
		}
	};
	
	static inline bool nonzero( char c )
	{
		return c;
	}
	
	static vfs::node_ptr IconSuite_Factory( const vfs::node*     parent,
	                                        const plus::string&  name,
	                                        const void*          args )
	{
		const Component comp = GetKeyFromParent( parent );
		
		n::owned< N::IconSuiteRef > iconSuite = N::GetComponentIconSuite( comp );
		
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
	
	static const vfs::property_params sys_mac_thng_REF_code_params =
	{
		sizeof (::OSType),  // fixed size
		(vfs::property_get_hook) &sys_mac_thng_REF_code_get,
	};
	
	#define DEFINE_PARAMS( p )  \
	static const vfs::property_params p##_params = {vfs::no_fixed_size, &p##_get}
	
	DEFINE_PARAMS( sys_mac_thng_REF_name );
	DEFINE_PARAMS( sys_mac_thng_REF_info );
	
	#define PROPERTY( prop )  &vfs::new_property, &prop##_params
	
	const vfs::fixed_mapping sys_mac_thng_REF_Mappings[] =
	{
		{ "type",         PROPERTY( sys_mac_thng_REF_code ) },
		{ "subtype",      PROPERTY( sys_mac_thng_REF_code ) },
		{ "manufacturer", PROPERTY( sys_mac_thng_REF_code ) },
		
		{ "name",         PROPERTY( sys_mac_thng_REF_name ) },
		{ "info",         PROPERTY( sys_mac_thng_REF_info ) },
		
		{ "icon",         &vfs::new_generated, (void*) &sys_mac_thng_REF_icon::Get },
		
	#if CONFIG_ICONSUITES
		
		{ "suite",        &IconSuite_Factory },
		
	#endif
		
		{ NULL, NULL }
	};
	
	vfs::node_ptr New_FSTree_sys_mac_thng( const vfs::node*     parent,
	                                       const plus::string&  name,
	                                       const void*          args )
	{
		enum
		{
			gestaltComponentMgr = 'cpnt',
		};
		
		if ( ! mac::sys::gestalt( gestaltComponentMgr ) )
		{
			p7::throw_errno( ENOENT );
		}
		
		return vfs::new_basic_directory( parent, name, thng_lookup, thng_iterate );
	}
	
}
