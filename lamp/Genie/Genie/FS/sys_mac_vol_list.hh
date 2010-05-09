/*
	sys_mac_vol_list.hh
	-------------------
	
	Joshua Juran
*/

#ifndef GENIE_FS_SYSMACVOLLIST_HH
#define GENIE_FS_SYSMACVOLLIST_HH

// Nitrogen
#include "Nitrogen/Files.hh"

// Genie
#include "Genie/FS/FSTree.hh"


namespace Genie
{
	
	FSTreePtr New_FSTree_sys_mac_vol( const FSTreePtr& parent, const plus::string& name );
	
	FSTreePtr Get_sys_mac_vol_N( Nitrogen::FSVolumeRefNum vRefNum );
	
}

#endif

