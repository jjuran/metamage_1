/*
	Genie/FS/sys/mac/gdev/list.cc
	-----------------------------
*/

#include "Genie/FS/sys/mac/gdev/list.hh"

// iota
#include "iota/decimal.hh"
#include "iota/hexidecimal.hh"

// plus
#include "plus/contains.hh"
#include "plus/hexidecimal.hh"
#include "plus/var_string.hh"

// nucleus
#include "nucleus/linked_list_sequence.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Nitrogen
#include "Nitrogen/Quickdraw.hh"

// MacFeatures
#include "MacFeatures/ColorQuickdraw.hh"

// Genie
#include "Genie/FS/basic_directory.hh"
#include "Genie/FS/FSTreeCache.hh"
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/serialize_qd.hh"
#include "Genie/FS/SymbolicLink.hh"
#include "Genie/Utilities/canonical_32_bit_hex.hh"


namespace Nitrogen
{
	
	class DeviceList_ContainerSpecifics
	{
		public:
			typedef GDHandle  value_type;
			typedef UInt32    size_type;
			typedef SInt32    difference_type;
			
			value_type get_next_value( value_type value )
			{
				return GetNextDevice( value );
			}
			
			static value_type begin_value()  { return GetDeviceList(); }
	};
	
	class DeviceList_Container: public nucleus::linked_list_sequence< ::Nitrogen::DeviceList_ContainerSpecifics >
	{
		friend DeviceList_Container DeviceList();
		
		private:
			DeviceList_Container()
			: nucleus::linked_list_sequence< ::Nitrogen::DeviceList_ContainerSpecifics >( ::Nitrogen::DeviceList_ContainerSpecifics() )
			{
			}
	};
	
	inline DeviceList_Container DeviceList()
	{
		return DeviceList_Container();
	}
	
}

namespace Genie
{
	
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	static N::GDHandle GetKeyFromParent( const FSTreePtr& parent )
	{
		return (N::GDHandle) plus::decode_32_bit_hex( parent->Name() );
	}
	
	static N::GDHandle GetKey( const FSTree* that )
	{
		return GetKeyFromParent( that->ParentRef() );
	}
	
	
	static bool is_valid_GDHandle_name( const plus::string& name )
	{
		if ( !canonical_32_bit_hex::applies( name ) )
		{
			return false;
		}
		
		const GDHandle gdH = (GDHandle) plus::decode_32_bit_hex( name );
		
		return plus::contains( N::DeviceList(), gdH );
	}
	
	
	extern const FSTree_Premapped::Mapping sys_mac_gdev_list_H_Mappings[];
	
	static FSTreePtr gdev_lookup( const FSTreePtr& parent, const plus::string& name )
	{
		if ( !is_valid_GDHandle_name( name ) )
		{
			poseven::throw_errno( ENOENT );
		}
		
		return Premapped_Factory( parent, name, sys_mac_gdev_list_H_Mappings );
	}
	
	class gdev_IteratorConverter
	{
		public:
			FSNode operator()( N::GDHandle gdH ) const
			{
				const ino_t inode = 0;
				
				plus::string name = plus::encode_32_bit_hex( (unsigned) gdH );
				
				return FSNode( inode, name );
			}
	};
	
	static void gdev_iterate( FSTreeCache& cache )
	{
		gdev_IteratorConverter converter;
		
		N::DeviceList_Container sequence = N::DeviceList();
		
		std::transform( sequence.begin(),
		                sequence.end(),
		                std::back_inserter( cache ),
		                converter );
	}
	
	
	struct GetGDBounds : serialize_Rect
	{
		typedef const Rect& Result;
		
		static Result Get( N::GDHandle gdevice )
		{
			return gdevice[0]->gdRect;
		}
	};
	
	struct GetGDSize : serialize_Point
	{
		typedef Point Result;
		
		static Result Get( N::GDHandle gdevice )
		{
			const Rect& bounds = gdevice[0]->gdRect;
			
			const Point size = { bounds.bottom - bounds.top,
			                     bounds.right - bounds.left };
			
			return size;
		}
	};
	
	template < class Accessor >
	struct sys_mac_gdev_list_N_Property : readonly_property
	{
		static const std::size_t fixed_size = Accessor::fixed_size;
		
		typedef N::GDHandle Key;
		
		static void get( plus::var_string& result, const FSTree* that, bool binary )
		{
			Key key = GetKey( that );
			
			const typename Accessor::Result data = Accessor::Get( key );
			
			Accessor::deconstruct::apply( result, data, binary );
		}
	};
	
	static FSTreePtr Driver_Link_Factory( const FSTreePtr&     parent,
	                                      const plus::string&  name,
	                                      const void*          args )
	{
		N::GDHandle key = GetKeyFromParent( parent );
		
		plus::string unit = iota::inscribe_decimal( ~key[0]->gdRefNum );
		
		return New_FSTree_SymbolicLink( parent, name, "/sys/mac/unit/" + unit );
	}
	
	#define PROPERTY( prop )  &new_property, &property_params_factory< sys_mac_gdev_list_N_Property< prop > >::value
	
	const FSTree_Premapped::Mapping sys_mac_gdev_list_H_Mappings[] =
	{
		{ "driver", &Driver_Link_Factory },
		
		{ "bounds", PROPERTY( GetGDBounds ) },
		
		{ NULL, NULL }
	};
	
	FSTreePtr New_FSTree_sys_mac_gdev_list( const FSTreePtr&     parent,
	                                        const plus::string&  name,
	                                        const void*          args )
	{
		if ( !MacFeatures::Has_ColorQuickdraw() )
		{
			p7::throw_errno( ENOENT );
		}
		
		return new_basic_directory( parent, name, gdev_lookup, gdev_iterate );
	}
	
}

