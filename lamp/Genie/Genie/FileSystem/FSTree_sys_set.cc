/*	=================
 *	FSTree_sys_set.cc
 *	=================
 */

#include "Genie/FileSystem/FSTree_sys_set.hh"

// Genie
#include "Genie/FileSystem/DynamicGroups.hh"
#include "Genie/FileSystem/FSTree_Directory.hh"
#include "Genie/FileSystem/FSTree_sys.hh"
#include "Genie/IO/BufferFile.hh"
#include "Genie/IO/GraphicsPort.hh"


namespace Genie
{
	
	typedef FSTree_Sequence< DynamicGroup_Details< GraphicsWindow   > > FSTree_sys_set_gfx;
	typedef FSTree_Sequence< DynamicGroup_Details< BufferFileHandle > > FSTree_sys_set_txt;
	
	
	void FSTree_sys_set::Init()
	{
		Map( "gfx", &Singleton_Factory< FSTree_sys_set_gfx > );
		Map( "txt", &Singleton_Factory< FSTree_sys_set_txt > );
	}
	
}

