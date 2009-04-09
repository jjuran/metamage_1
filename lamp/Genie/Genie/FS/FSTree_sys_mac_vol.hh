/*	=====================
 *	FSTree_sys_mac_vol.hh
 *	=====================
 */

#ifndef GENIE_FILESYSTEM_FSTREESYSMACVOL_HH
#define GENIE_FILESYSTEM_FSTREESYSMACVOL_HH

// Nitrogen
#include "Nitrogen/Files.h"

// Genie
#include "Genie/FS/FSTree.hh"


namespace Genie
{
	
	FSTreePtr New_FSTree_sys_mac_vol( const FSTreePtr& parent, const std::string& name );
	
	FSTreePtr Get_sys_mac_vol_N( Nitrogen::FSVolumeRefNum vRefNum );
	
}

#endif

