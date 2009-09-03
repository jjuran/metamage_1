/*	======================
 *	FSTree_sys_mac_gdev.cc
 *	======================
 */

#include "Genie/FS/FSTree_sys_mac_gdev.hh"

// Iota
#include "iota/decimal.hh"

// Nucleus
#include "Nucleus/LinkedListContainer.h"

// Nitrogen
#include "Nitrogen/QuickDraw.h"

// Genie
#include "Genie/FS/FSTree_Directory.hh"
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/FSTree_Virtual_Link.hh"
#include "Genie/FS/Scribes.hh"


namespace Nitrogen
{
	
	class DeviceList_ContainerSpecifics
	{
		public:
			typedef GDHandle  value_type;
			typedef UInt32    size_type;
			typedef SInt32    difference_type;
			
			value_type GetNextValue( value_type value )
			{
				return GetNextDevice( value );
			}
			
			static value_type begin_value()  { return GetDeviceList(); }
	};
	
	class DeviceList_Container: public Nucleus::LinkedListContainer< ::Nitrogen::DeviceList_ContainerSpecifics >
	{
		friend DeviceList_Container DeviceList();
		
		private:
			DeviceList_Container()
			: Nucleus::LinkedListContainer< ::Nitrogen::DeviceList_ContainerSpecifics >( ::Nitrogen::DeviceList_ContainerSpecifics() )
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
	
	
	struct GDHandle_KeyName_Traits : public Pointer_KeyName_Traits< Nitrogen::GDHandle >  {};
	
	struct sys_mac_gdev_Details : public GDHandle_KeyName_Traits
	{
		typedef Nitrogen::DeviceList_Container Sequence;
		
		static Sequence ItemSequence()  { return Nitrogen::DeviceList(); }
		
		static Key KeyFromValue( const Sequence::value_type& value )  { return value; }
		
		static bool KeyIsValid( const Key& key );
		
		static FSTreePtr GetChildNode( const FSTreePtr&    parent,
		                               const std::string&  name,
		                               const Key&          key );
	};
	
	typedef FSTree_Sequence< sys_mac_gdev_Details > FSTree_sys_mac_gdev;
	
	
	inline unsigned char nibble_from_ascii( char c )
	{
		return c & 0x10 ?  c         - '0'
		                : (c | 0x20) - 'a' + 10;
	}
	
	static inline UInt32 Read_8_nibbles( const char* p )
	{
		UInt32 result = nibble_from_ascii( p[ 0 ] ) << 28
		              | nibble_from_ascii( p[ 1 ] ) << 24
		              | nibble_from_ascii( p[ 2 ] ) << 20
		              | nibble_from_ascii( p[ 3 ] ) << 16
		              | nibble_from_ascii( p[ 4 ] ) << 12
		              | nibble_from_ascii( p[ 5 ] ) <<  8
		              | nibble_from_ascii( p[ 6 ] ) <<  4
		              | nibble_from_ascii( p[ 7 ] ) <<  0;
		
		return result;
	}
	
	
	static inline void* PtrFromName( const std::string& name )
	{
		if ( name.length() != sizeof (void*) * 2 )
		{
			return NULL;
		}
		
		return (void*) Read_8_nibbles( name.data() );
	}
	
	static N::GDHandle GetKeyFromParent( const FSTreePtr& parent )
	{
		return (N::GDHandle) PtrFromName( parent->Name() );
	}
	
	static N::GDHandle GetKey( const FSTree* that )
	{
		return GetKeyFromParent( that->ParentRef() );
	}
	
	
	bool sys_mac_gdev_Details::KeyIsValid( const Key& key )
	{
		Sequence sequence = ItemSequence();
		
		return std::find( sequence.begin(), sequence.end(), key ) != sequence.end();
	}
	
	
	extern const FSTree_Premapped::Mapping sys_mac_gdev_H_Mappings[];
	
	FSTreePtr sys_mac_gdev_Details::GetChildNode( const FSTreePtr&    parent,
		                                          const std::string&  name,
		                                          const Key&          key )
	{
		return Premapped_Factory< sys_mac_gdev_H_Mappings >( parent, name );
	}
	
	
	struct GetGDBounds
	{
		typedef std::string Result;
		
		static Result Get( N::GDHandle gdevice, bool binary )
		{
			return Freeze< Rect_Scribe >( gdevice[0]->gdRect, binary );
		}
	};
	
	struct GetGDSize
	{
		typedef std::string Result;
		
		static Result Get( N::GDHandle gdevice, bool binary )
		{
			const Rect& bounds = gdevice[0]->gdRect;
			
			const Point size = { bounds.bottom - bounds.top,
			                     bounds.right - bounds.left };
			
			return Freeze< Point_Scribe< 'x' > >( size, binary );
		}
	};
	
	template < class Accessor >
	struct sys_mac_gdev_N_Property
	{
		typedef N::GDHandle Key;
		
		static std::string Read( const FSTree* that, bool binary )
		{
			Key key = GetKey( that );
			
			return NN::Convert< std::string >( Accessor::Get( key, binary ) );
		}
	};
	
	template < class Accessor >
	static FSTreePtr Property_Factory( const FSTreePtr&    parent,
	                                   const std::string&  name )
	{
		typedef sys_mac_gdev_N_Property< Accessor > Property;
		
		return New_FSTree_Property( parent,
		                            name,
		                            &Property::Read );
	}
	
	static FSTreePtr Driver_Link_Factory( const FSTreePtr&    parent,
	                                      const std::string&  name )
	{
		GDHandle_KeyName_Traits::Key key = GetKeyFromParent( parent );
		
		std::string unit = iota::inscribe_decimal( ~key[0]->gdRefNum );
		
		return New_FSTree_Virtual_Link( parent, name, "/sys/mac/unit/" + unit );
	}
	
	const FSTree_Premapped::Mapping sys_mac_gdev_H_Mappings[] =
	{
		{ "driver", &Driver_Link_Factory },
		
		{ "bounds", &Property_Factory< GetGDBounds > },
		
		{ NULL, NULL }
	};
	
	FSTreePtr New_FSTree_sys_mac_gdev( const FSTreePtr& parent, const std::string& name )
	{
		return FSTreePtr( new FSTree_sys_mac_gdev( parent, name ) );
	}
	
}

