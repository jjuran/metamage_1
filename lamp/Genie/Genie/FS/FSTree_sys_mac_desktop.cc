/*	=========================
 *	FSTree_sys_mac_desktop.cc
 *	=========================
 */

#include "Genie/FS/FSTree_sys_mac_desktop.hh"

// Nucleus
#include "Nucleus/Convert.h"

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
		typedef std::string Result;
		
		static Result Get( const BitMap& screenBits, bool binary )
		{
			return Freeze< Rect_Scribe >( screenBits.bounds, binary );
		}
	};
	
	struct GetScreenSize
	{
		typedef std::string Result;
		
		static Result Get( const BitMap& screenBits, bool binary )
		{
			const Rect& bounds = screenBits.bounds;
			
			const Point size = { bounds.bottom - bounds.top,
			                     bounds.right - bounds.left };
			
			return Freeze< Point_Scribe< 'x' > >( size, binary );
		}
	};
	
	template < class Accessor >
	struct sys_mac_desktop_Property
	{
		static std::string Read( const FSTree* that, bool binary )
		{
			const BitMap& screenBits = N::GetQDGlobalsScreenBits();
			
			return Accessor::Get( screenBits, binary );
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

