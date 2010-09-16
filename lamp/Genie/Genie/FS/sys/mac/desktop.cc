/*
	Genie/FS/sys/mac/desktop.cc
	---------------------------
*/

#include "Genie/FS/sys/mac/desktop.hh"

// plus
#include "plus/var_string.hh"

// Nitrogen
#include "Nitrogen/Quickdraw.hh"

// Genie
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/serialize_qd.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	
	struct GetScreenBounds : serialize_Rect
	{
		typedef const Rect& Result;
		
		static Result Get( const BitMap& screenBits )
		{
			return screenBits.bounds;
		}
	};
	
	struct GetScreenSize : serialize_Point
	{
		typedef Point Result;
		
		static Result Get( const BitMap& screenBits )
		{
			const Rect& bounds = screenBits.bounds;
			
			const Point size = { bounds.bottom - bounds.top,
			                     bounds.right - bounds.left };
			
			return size;
		}
	};
	
	template < class Accessor >
	struct sys_mac_desktop_Property
	{
		static void Read( plus::var_string& result, const FSTree* that, bool binary )
		{
			const BitMap& screenBits = N::GetQDGlobalsScreenBits();
			
			const typename Accessor::Result data = Accessor::Get( screenBits );
			
			Accessor::deconstruct::apply( result, data, binary );
		}
	};
	
	template < class Accessor >
	static FSTreePtr Property_Factory( const FSTreePtr&     parent,
	                                   const plus::string&  name,
	                                   const void*          args )
	{
		typedef sys_mac_desktop_Property< Accessor > Property;
		
		return New_FSTree_Property( parent,
		                            name,
		                            &Property::Read );
	}
	
	const FSTree_Premapped::Mapping sys_mac_desktop_Mappings[] =
	{
		{ "bounds", &Property_Factory< GetScreenBounds > },
		{ "size",   &Property_Factory< GetScreenSize   > },
		
		{ NULL, NULL }
	};
	
}

