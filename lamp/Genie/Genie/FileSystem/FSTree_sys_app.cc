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
#include "Genie/FileSystem/FSTree_QueryFile.hh"
#include "Genie/FileSystem/FSTree_sys_mac_window.hh"


namespace Genie
{
	
	namespace NN = Nucleus;
	
	
	struct GetFreeMem
	{
		typedef long Result;
		
		Result Get() const
		{
			return ::FreeMem();
		}
	};
	
#if !TARGET_API_MAC_CARBON
	
	struct GetHeapSize
	{
		typedef long Result;
		
		Result Get() const
		{
			THz zone = ::ApplicationZone();
			
			return zone->bkLim - (Ptr) &zone->heapData;
		}
	};
	
#endif
	
	template < class Accessor >
	class sys_app_Query
	{
		public:
			std::string Get() const
			{
				std::string output = NN::Convert< std::string >( Accessor().Get() ) + "\n";
				
				return output;
			}
	};
	
	template < class Accessor >
	static FSTreePtr Query_Factory( const FSTreePtr&    parent,
	                                const std::string&  name )
	{
		typedef sys_app_Query< Accessor > Query;
		
		typedef FSTree_QueryFile< Query > QueryFile;
		
		return FSTreePtr( new QueryFile( parent, name ) );
	}
	
	const Singleton_Mapping sys_app_Mappings[] =
	{
		{ "freemem", &Query_Factory< GetFreeMem > },
		
	#if !TARGET_API_MAC_CARBON
		
		{ "heapsize", &Query_Factory< GetHeapSize > },
		
	#endif
		
		{ "window", &Singleton_Factory< FSTree_sys_mac_window > },
		
		{ NULL, NULL }
		
	};
	
}

