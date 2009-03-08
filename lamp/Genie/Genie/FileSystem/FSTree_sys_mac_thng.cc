/*	======================
 *	FSTree_sys_mac_thng.cc
 *	======================
 */

#include "Genie/FileSystem/FSTree_sys_mac_thng.hh"

// Standard C++
#include <algorithm>

// POSeven
#include "POSeven/Errno.hh"

// Genie
#include "Genie/FileSystem/FSSpec.hh"
#include "Genie/FileSystem/FSTree_Generated.hh"
#include "Genie/FileSystem/FSTree_IconSuite.hh"
#include "Genie/FileSystem/FSTree_Property.hh"


namespace Nitrogen
{
	
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
		try
		{
			(void) N::GetComponentInfo( key );
		}
		catch ( const N::BadComponentSelector& err )
		{
		#ifdef __MWERKS__
			
			if ( err.Get() != badComponentSelector )
			{
				throw;
			}
			
		#endif
			
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
		return FSTreePtr( new FSTree_Generated( parent,
		                                        name,
		                                        &Property::Get ) );
	}
	
	template < class Property >
	static FSTreePtr Property_Factory( const FSTreePtr&    parent,
	                                   const std::string&  name )
	{
		return FSTreePtr( new FSTree_Property( parent,
		                                       name,
		                                       &Property::Get ) );
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
	
}

