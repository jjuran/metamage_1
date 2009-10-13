/*	=========================
 *	FSTree_sys_mac_desktop.cc
 *	=========================
 */

#include "Genie/FS/FSTree_sys_mac_desktop.hh"

// Nitrogen
#include "Nitrogen/QuickDraw.h"

// Genie
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/Scribes.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	
	struct GetScreenBounds
	{
		typedef const Rect& Result;
		
		typedef Rect_Scribe Scribe;
		
		static Result Get( const BitMap& screenBits )
		{
			return screenBits.bounds;
		}
	};
	
	struct GetScreenSize
	{
		typedef Point Result;
		
		typedef Point_Scribe< 'x' > Scribe;
		
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
		static std::string Read( const FSTree* that, bool binary )
		{
			const BitMap& screenBits = N::GetQDGlobalsScreenBits();
			
			const typename Accessor::Result data = Accessor::Get( screenBits );
			
			return Freeze< Accessor::Scribe >( data, binary );
		}
	};
	
	template < class Accessor >
	static FSTreePtr Property_Factory( const FSTreePtr&    parent,
	                                   const std::string&  name )
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

