/*
	Genie/FS/sys/mac/vol/list.hh
	----------------------------
*/

#ifndef GENIE_FS_SYS_MAC_VOL_LIST_HH
#define GENIE_FS_SYS_MAC_VOL_LIST_HH

// Nitrogen
#ifndef MAC_FILES_TYPES_FSVOLUMEREFNUM_HH
#include "Mac/Files/Types/FSVolumeRefNum.hh"
#endif

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

