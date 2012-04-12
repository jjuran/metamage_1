/*
	Genie/FS/sys/mac/gdev/list.cc
	-----------------------------
*/

#include "Genie/FS/sys/mac/gdev/list.hh"

// gear
#include "gear/inscribe_decimal.hh"

// plus
#include "plus/contains.hh"
#include "plus/hexidecimal.hh"
#include "plus/var_string.hh"
#include "plus/string/concat.hh"

// nucleus
#include "nucleus/linked_list_sequence.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Nitrogen
#include "Nitrogen/Quickdraw.hh"

// MacFeatures
#include "MacFeatures/ColorQuickdraw.hh"

// vfs
#include "vfs/dir_contents.hh"
#include "vfs/dir_entry.hh"

// Genie
#include "Genie/FS/basic_directory.hh"
#include "Genie/FS/FSTree_Directory.hh"
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/property.hh"
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
	
	
	static GDHandle GetKeyFromParent( const FSTree* parent )
	{
		return (GDHandle) plus::decode_32_bit_hex( parent->Name() );
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
	
	
	extern const premapped::mapping sys_mac_gdev_list_H_Mappings[];
	
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
			vfs::dir_entry operator()( GDHandle gdH ) const
			{
				const ino_t inode = 0;
				
				plus::string name = plus::encode_32_bit_hex( (unsigned) gdH );
				
				return vfs::dir_entry( inode, name );
			}
	};
	
	static void gdev_iterate( const FSTreePtr& parent, vfs::dir_contents& cache )
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
		
		static Result Get( GDHandle gdevice )
		{
			return gdevice[0]->gdRect;
		}
	};
	
	struct GetGDSize : serialize_Point
	{
		typedef Point Result;
		
		static Result Get( GDHandle gdevice )
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
		
		typedef GDHandle Key;
		
		static void get( plus::var_string& result, const FSTree* that, bool binary )
		{
			Key key = GetKeyFromParent( that );
			
			const typename Accessor::Result data = Accessor::Get( key );
			
			Accessor::deconstruct::apply( result, data, binary );
		}
	};
	
	static FSTreePtr Driver_Link_Factory( const FSTreePtr&     parent,
	                                      const plus::string&  name,
	                                      const void*          args )
	{
		GDHandle key = GetKeyFromParent( parent.get() );
		
		plus::string unit = gear::inscribe_decimal( ~key[0]->gdRefNum );
		
		unit = "/sys/mac/unit/" + unit;
		
		return New_FSTree_SymbolicLink( parent, name, unit );
	}
	
	#define PROPERTY( prop )  &new_property, &property_params_factory< sys_mac_gdev_list_N_Property< prop > >::value
	
	const premapped::mapping sys_mac_gdev_list_H_Mappings[] =
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

