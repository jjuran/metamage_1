/*	=====================
 *	FSTree_sys_mac_adb.hh
 *	=====================
 */

#ifndef GENIE_FILESYSTEM_FSTREESYSMACADB_HH
#define GENIE_FILESYSTEM_FSTREESYSMACADB_HH

// ClassicToolbox
#include "ClassicToolbox/DeskBus.h"

// Genie
#include "Genie/FileSystem/FSTree_sys_mac.hh"


namespace Genie
{
	
	struct ADBAddress_KeyName_Traits : Integer_KeyName_Traits< Nitrogen::ADBAddress >
	{
	};
	
	struct sys_mac_adb_Details : public ADBAddress_KeyName_Traits
	{
		typedef Nitrogen::ADBDevice_Container Sequence;
		
		static Sequence ItemSequence()  { return Nitrogen::ADBDevice_Container(); }
		
		static Key KeyFromValue( Sequence::const_reference ref )  { return ref; }
		
		static bool KeyIsValid( const Key& key );
		
		static FSTreePtr GetChildNode( const FSTreePtr&    parent,
		                               const std::string&  name,
		                               const Key&          key );
	};
	
	typedef FSTree_Sequence< sys_mac_adb_Details > FSTree_sys_mac_adb;
	
}

#endif

