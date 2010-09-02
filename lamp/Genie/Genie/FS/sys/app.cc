/*
	Genie/FS/sys/app.cc
	-------------------
*/

#include "Genie/FS/sys/app.hh"

#ifndef __MACMEMORY__
#include <MacMemory.h>
#endif

// plus
#include "plus/serialize.hh"

// Genie
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/sys/app/dir.hh"
#include "Genie/FS/sys/app/exe.hh"
#include "Genie/FS/sys/app/window.hh"


namespace Genie
{
	
	struct GetFreeMem : plus::serialize_unsigned< long >
	{
		static long Get()
		{
			return ::FreeMem();
		}
	};
	
#if !TARGET_API_MAC_CARBON
	
	struct GetHeapSize : plus::serialize_unsigned< long >
	{
		static long Get()
		{
			THz zone = ::ApplicationZone();
			
			return zone->bkLim - (Ptr) &zone->heapData;
		}
	};
	
#endif
	
	template < class Accessor >
	struct sys_app_Property
	{
		typedef typename Accessor::result_type result_type;
		
		static void Read( plus::var_string& result, const FSTree* that, bool binary )
		{
			const result_type data = Accessor::Get();
			
			Accessor::deconstruct::apply( result, data, binary );
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
		                            sizeof (typename Accessor::result_type),
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

