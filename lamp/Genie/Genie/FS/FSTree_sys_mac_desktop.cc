/*	=========================
 *	FSTree_sys_mac_desktop.cc
 *	=========================
 */

#include "Genie/FS/FSTree_sys_mac_desktop.hh"

// Nitrogen
#include "Nitrogen/Quickdraw.hh"

// Genie
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/stringify_qd.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	
	struct GetScreenBounds
	{
		typedef stringify_Rect stringify;
		
		typedef const Rect& Result;
		
		static Result Get( const BitMap& screenBits )
		{
			return screenBits.bounds;
		}
	};
	
	struct GetScreenSize
	{
		typedef stringify_Point stringify;
		
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
		static plus::string Read( const FSTree* that, bool binary )
		{
			const BitMap& screenBits = N::GetQDGlobalsScreenBits();
			
			const typename Accessor::Result data = Accessor::Get( screenBits );
			
			return Accessor::stringify::apply( data, binary );
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

