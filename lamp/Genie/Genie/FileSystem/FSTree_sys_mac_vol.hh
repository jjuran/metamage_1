/*	=====================
 *	FSTree_sys_mac_vol.hh
 *	=====================
 */

#ifndef GENIE_FILESYSTEM_FSTREESYSMACVOL_HH
#define GENIE_FILESYSTEM_FSTREESYSMACVOL_HH

// Nitrogen
#include "Nitrogen/Files.h"

// Genie
#include "Genie/FileSystem/FSTree_Directory.hh"


namespace Genie
{
	
	struct VRefNum_KeyName_Traits
	{
		typedef Nitrogen::FSVolumeRefNum Key;
		
		static std::string NameFromKey( const Key& key );
		
		static Key KeyFromName( const std::string& name );
		
		static bool KeyIsValid( const Key& key );
	};
	
	
	struct sys_mac_vol_Details : public VRefNum_KeyName_Traits
	{
		typedef Nitrogen::Volume_Container Sequence;
		
		static const Sequence& ItemSequence()  { return Nitrogen::Volumes(); }
		
		static Key KeyFromValue( const Sequence::value_type& value )  { return value; }
		
		static FSTreePtr GetChildNode( const FSTreePtr&    parent,
		                               const std::string&  name,
		                               const Key&          key );
	};
	
	typedef FSTree_Sequence< sys_mac_vol_Details > FSTree_sys_mac_vol;
	
}

#endif

