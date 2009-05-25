/*	=================
 *	FSTree_sys_app.cc
 *	=================
 */

#include "Genie/FS/FSTree_sys_app.hh"

#ifndef __MACMEMORY__
#include <MacMemory.h>
#endif

// Nucleus
#include "Nucleus/Convert.h"

// Genie
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/sys_app_window.hh"
#include "Genie/FS/Scribes.hh"


namespace Genie
{
	
	namespace NN = Nucleus;
	
	
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
		static std::string Read( const FSTree* that, bool binary )
		{
			return Freeze< Integer_Scribe< long > >( Accessor::Get(), binary );
		}
	};
	
	template < class Accessor >
	static FSTreePtr Property_Factory( const FSTreePtr&    parent,
	                                   const std::string&  name )
	{
		typedef sys_app_Property< Accessor > Property;
		
		return New_FSTree_Property( parent,
		                            name,
		                            &Property::Read );
	}
	
	const FSTree_Premapped::Mapping sys_app_Mappings[] =
	{
		{ "freemem", &Property_Factory< GetFreeMem > },
		
	#if !TARGET_API_MAC_CARBON
		
		{ "heapsize", &Property_Factory< GetHeapSize > },
		
	#endif
		
		{ "window", &Premapped_Factory< sys_app_window_Mappings > },
		
		{ NULL, NULL }
		
	};
	
}

