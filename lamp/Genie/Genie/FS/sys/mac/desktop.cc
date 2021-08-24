/*
	Genie/FS/sys/mac/desktop.cc
	---------------------------
*/

#include "Genie/FS/sys/mac/desktop.hh"

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// mac-qd-utils
#include "mac_qd/globals/screenBits.hh"

// plus
#include "plus/var_string.hh"

// vfs
#include "vfs/property.hh"
#include "vfs/node/types/property_file.hh"

// Genie
#include "Genie/FS/serialize_qd.hh"


namespace Genie
{
	
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
	
	static
	void GetScreenBounds_get( plus::var_string& result, const vfs::node* that, bool binary )
	{
		typedef serialize_Rect Accessor;
		
		const BitMap& screenBits = mac::qd::screenBits();
		
		Accessor::deconstruct::apply( result, screenBits.bounds, binary );
	}
	
	static
	void GetScreenSize_get( plus::var_string& result, const vfs::node* that, bool binary )
	{
		typedef GetScreenSize Accessor;
		
		const BitMap& screenBits = mac::qd::screenBits();
		
		const Point data = GetScreenSize::Get( screenBits );
		
		Accessor::deconstruct::apply( result, data, binary );
	}
	
	static const vfs::property_params GetScreenBounds_params =
	{
		serialize_Rect::fixed_size,
		&GetScreenBounds_get,
	};
	
	static const vfs::property_params GetScreenSize_params =
	{
		GetScreenSize::fixed_size,
		&GetScreenSize_get,
	};
	
	#define PROPERTY( prop )  &vfs::new_property, &prop##_params
	
	const vfs::fixed_mapping sys_mac_desktop_Mappings[] =
	{
		{ "bounds", PROPERTY( GetScreenBounds ) },
		{ "size",   PROPERTY( GetScreenSize   ) },
		
		{ NULL, NULL }
	};
	
}
