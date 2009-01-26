/*	======================
 *	FSTree_sys_mac_gdev.cc
 *	======================
 */

#include "Genie/FileSystem/FSTree_sys_mac_gdev.hh"

// Genie
#include "Genie/FileSystem/FSTree_Property.hh"
#include "Genie/FileSystem/FSTree_Virtual_Link.hh"
#include "Genie/FileSystem/Scribes.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	
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
		
		return FSTreePtr( new FSTree_Property( parent,
		                                       name,
		                                       &Property::Read ) );
	}
	
	static FSTreePtr Driver_Link_Factory( const FSTreePtr&    parent,
	                                      const std::string&  name )
	{
		GDHandle_KeyName_Traits::Key key = GetKeyFromParent( parent );
		
		std::string unit = NN::Convert< std::string >( ~key[0]->gdRefNum );
		
		return FSTreePtr( new FSTree_Virtual_Link( parent, name, "/sys/mac/unit/" + unit ) );
	}
	
	const FSTree_Premapped::Mapping sys_mac_gdev_H_Mappings[] =
	{
		{ "driver", &Driver_Link_Factory },
		
		{ "bounds", &Property_Factory< GetGDBounds > },
		
		{ NULL, NULL }
	};
	
}

