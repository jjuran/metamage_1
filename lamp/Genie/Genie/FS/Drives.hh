/*	=========
 *	Drives.hh
 *	=========
 */

#ifndef GENIE_FILESYSTEM_DRIVES_HH
#define GENIE_FILESYSTEM_DRIVES_HH

// Genie
#include "Genie/FS/FSTree_fwd.hh"


namespace Genie
{
	
	void volume_flush_trigger( const FSTree* that );
	void volume_eject_trigger( const FSTree* that );
	void volume_mount_trigger( const FSTree* that );
	void volume_unmount_trigger( const FSTree* that );
	
}

#endif

