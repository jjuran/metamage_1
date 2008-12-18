/*	=================
 *	FSTree_sys_set.cc
 *	=================
 */

#include "Genie/FileSystem/FSTree_sys_set.hh"

// Genie
#include "Genie/FileSystem/DynamicGroups.hh"
#include "Genie/IO/BufferFile.hh"


namespace Genie
{
	
	typedef FSTree_Sequence< DynamicGroup_Details< BufferFileHandle > > FSTree_sys_set_txt;
	
	
	const Singleton_Mapping sys_set_Mappings[] =
	{
		{ "txt", &Singleton_Factory< FSTree_sys_set_txt  > },
		
		{ NULL, NULL }
	};
	
}

