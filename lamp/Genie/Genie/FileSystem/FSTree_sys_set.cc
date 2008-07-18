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
	
	template <>
	struct IOHandle_Name_Traits< GraphicsWindow >
	{
		static std::string Name()  { return "gfx"; }
	};
	
	template <>
	struct IOHandle_Name_Traits< BufferFileHandle >
	{
		static std::string Name()  { return "txt"; }
	};
	
	
	template <>
	struct IOHandle_Parent_Traits< GraphicsWindow >
	{
		typedef FSTree_sys_set Tree;
	};
	
	template <>
	struct IOHandle_Parent_Traits< BufferFileHandle >
	{
		typedef FSTree_sys_set Tree;
	};
	
	
	typedef FSTree_Sequence< DynamicGroup_Details< GraphicsWindow   > > FSTree_sys_set_gfx;
	typedef FSTree_Sequence< DynamicGroup_Details< BufferFileHandle > > FSTree_sys_set_txt;
	
	
	template <>
	struct IOHandle_FSTree_Traits< GraphicsWindow >
	{
		typedef FSTree_sys_set_gfx Tree;
	};
	
	template <>
	struct IOHandle_FSTree_Traits< BufferFileHandle >
	{
		typedef FSTree_sys_set_txt Tree;
	};
	
	void FSTree_sys_set::Init()
	{
		Map( "gfx", &Singleton_Factory< FSTree_sys_set_gfx > );
		Map( "txt", &Singleton_Factory< FSTree_sys_set_txt > );
	}
	
}

