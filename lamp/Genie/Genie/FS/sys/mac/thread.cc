/*
	sys/mac/thread.cc
	-----------------
*/

#include "Genie/FS/sys/mac/thread.hh"

// Genie
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/sys/mac/thread/default-stack-size.hh"


namespace Genie
{
	
	static FSTreePtr Property_Factory( const FSTreePtr&     parent,
	                                   const plus::string&  name,
	                                   const void*          args )
	{
		return New_FSTree_Property( parent,
		                            name,
		                            (Property_ReadHook) args );
	}
	
	const FSTree_Premapped::Mapping sys_mac_thread_Mappings[] =
	{
		{ "default-stack-size", &Property_Factory, (void*) &sys_mac_thread_defaultstacksize::Read },
		
		{ NULL, NULL }
		
	};
	
}

