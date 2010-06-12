/*
	sys_mac_gdev_list.cc
	--------------------
	
	Joshua Juran
*/

#include "Genie/FS/sys_mac_gdev_list.hh"

// iota
#include "iota/decimal.hh"
#include "iota/hexidecimal.hh"

// plus
#include "plus/contains.hh"
#include "plus/hexidecimal.hh"

// nucleus
#include "nucleus/linked_list_sequence.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Nitrogen
#include "Nitrogen/Quickdraw.hh"

// Genie
#include "Genie/FS/basic_directory.hh"
#include "Genie/FS/FSTreeCache.hh"
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/Scribes.hh"
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
		
		return Premapped_Factory< sys_mac_gdev_list_H_Mappings >( parent, name );
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
	
	
	struct GetGDBounds
	{
		typedef const Rect& Result;
		
		typedef Rect_Scribe Scribe;
		
		static Result Get( N::GDHandle gdevice )
		{
			return gdevice[0]->gdRect;
		}
	};
	
	struct GetGDSize
	{
		typedef Point Result;
		
		typedef Point_Scribe< 'x' > Scribe;
		
		static Result Get( N::GDHandle gdevice )
		{
			const Rect& bounds = gdevice[0]->gdRect;
			
			const Point size = { bounds.bottom - bounds.top,
			                     bounds.right - bounds.left };
			
			return size;
		}
	};
	
	template < class Accessor >
	struct sys_mac_gdev_list_N_Property
	{
		typedef N::GDHandle Key;
		
		static plus::string Read( const FSTree* that, bool binary )
		{
			Key key = GetKey( that );
			
			const typename Accessor::Result data = Accessor::Get( key );
			
			return Freeze< typename Accessor::Scribe >( data, binary );
		}
	};
	
	template < class Accessor >
	static FSTreePtr Property_Factory( const FSTreePtr&     parent,
	                                   const plus::string&  name )
	{
		typedef sys_mac_gdev_list_N_Property< Accessor > Property;
		
		return New_FSTree_Property( parent,
		                            name,
		                            &Property::Read );
	}
	
	static FSTreePtr Driver_Link_Factory( const FSTreePtr&     parent,
	                                      const plus::string&  name )
	{
		N::GDHandle key = GetKeyFromParent( parent );
		
		plus::string unit = iota::inscribe_decimal( ~key[0]->gdRefNum );
		
		return New_FSTree_SymbolicLink( parent, name, "/sys/mac/unit/" + unit );
	}
	
	const FSTree_Premapped::Mapping sys_mac_gdev_list_H_Mappings[] =
	{
		{ "driver", &Driver_Link_Factory },
		
		{ "bounds", &Property_Factory< GetGDBounds > },
		
		{ NULL, NULL }
	};
	
	FSTreePtr New_FSTree_sys_mac_gdev_list( const FSTreePtr& parent, const plus::string& name )
	{
		return new_basic_directory( parent, name, gdev_lookup, gdev_iterate );
	}
	
}

