/*	=================
 *	FSTree_sys_app.cc
 *	=================
 */

#include "Genie/FS/FSTree_sys_app.hh"

#ifndef __MACMEMORY__
#include <MacMemory.h>
#endif

// Genie
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/sys_app_dir.hh"
#include "Genie/FS/sys_app_exe.hh"
#include "Genie/FS/sys_app_window.hh"
#include "Genie/FS/Scribes.hh"


namespace Genie
{
	
	struct GetFreeMem
	{
		typedef long Result;
		
		static Result Get()
		{
			return ::FreeMem();
		}
	};
	
#if !TARGET_API_MAC_CARBON
	
	struct GetHeapSize
	{
		typedef long Result;
		
		static Result Get()
		{
			THz zone = ::ApplicationZone();
			
			return zone->bkLim - (Ptr) &zone->heapData;
		}
	};
	
#endif
	
	template < class Accessor >
	struct sys_app_Property
	{
		static plus::string Read( const FSTree* that, bool binary )
		{
			return Freeze< Integer_Scribe< long > >( Accessor::Get(), binary );
		}
	};
	
	template < class Accessor >
	static FSTreePtr Property_Factory( const FSTreePtr&     parent,
	                                   const plus::string&  name,
	                                   const void*          args )
	{
		typedef sys_app_Property< Accessor > Property;
		
		return New_FSTree_Property( parent,
		                            name,
		                            sizeof (typename Accessor::Result),
		                            &Property::Read );
	}
	
	const FSTree_Premapped::Mapping sys_app_Mappings[] =
	{
		{ "dir",   &New_FSTree_sys_app_dir },
		{ "exe",   &New_FSTree_sys_app_exe },
		
		{ "freemem", &Property_Factory< GetFreeMem > },
		
	#if !TARGET_API_MAC_CARBON
		
		{ "heapsize", &Property_Factory< GetHeapSize > },
		
	#endif
		
		{ "window", &Premapped_Factory< sys_app_window_Mappings > },
		
		{ NULL, NULL }
		
	};
	
}

