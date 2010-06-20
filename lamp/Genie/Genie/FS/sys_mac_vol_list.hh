/*
	sys_mac_vol_list.hh
	-------------------
	
	Joshua Juran
*/

#ifndef GENIE_FS_SYSMACVOLLIST_HH
#define GENIE_FS_SYSMACVOLLIST_HH

// Nitrogen
#include "Mac/Files/Types/FSVolumeRefNum.hh"

// Genie
#include "Genie/FS/FSTree.hh"


namespace Genie
{
	
	FSTreePtr New_FSTree_sys_mac_vol( const FSTreePtr&     parent,
	                                  const plus::string&  name,
	                                  const void*          args );
	
	FSTreePtr Get_sys_mac_vol_N( Mac::FSVolumeRefNum vRefNum );
	
}

#endif

