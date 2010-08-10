/*
	sys/mac/time.cc
	---------------
*/

#include "Genie/FS/sys/mac/time.hh"

// Genie
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/sys/mac/time/dls+gmt-delta.hh"


namespace Genie
{
	
	template < class Property >
	static FSTreePtr Property_Factory( const FSTreePtr&     parent,
	                                   const plus::string&  name,
	                                   const void*          args )
	{
		return New_FSTree_Property( parent,
		                            name,
		                            &Property::Read );
	}
	
	const FSTree_Premapped::Mapping sys_mac_time_Mappings[] =
	{
		{ ".dls+gmt-delta", &Property_Factory< sys_mac_time_dlsgmtdelta > },
		
		{ NULL, NULL }
		
	};
	
}

