/*	=================
 *	FSTree_sys_app.cc
 *	=================
 */

#include "Genie/FileSystem/FSTree_sys_app.hh"

#ifndef __MACMEMORY__
#include <MacMemory.h>
#endif

// Nucleus
#include "Nucleus/Convert.h"

// Genie
#include "Genie/FileSystem/FSTree_Property.hh"
#include "Genie/FileSystem/FSTree_sys_mac_window.hh"


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
		static std::string Read( const FSTree* )
		{
			return NN::Convert< std::string >( Accessor::Get() );
		}
	};
	
	template < class Accessor >
	static FSTreePtr Property_Factory( const FSTreePtr&    parent,
	                                   const std::string&  name )
	{
		typedef sys_app_Property< Accessor > Property;
		
		return FSTreePtr( new FSTree_Property( parent,
		                                       name,
		                                       &Property::Read ) );
	}
	
	const FSTree_Premapped::Mapping sys_app_Mappings[] =
	{
		{ "freemem", &Property_Factory< GetFreeMem > },
		
	#if !TARGET_API_MAC_CARBON
		
		{ "heapsize", &Property_Factory< GetHeapSize > },
		
	#endif
		
		{ "window", &Basic_Factory< FSTree_sys_mac_window > },
		
		{ NULL, NULL }
		
	};
	
}

