/*	=========
 *	Drives.hh
 *	=========
 */

#ifndef GENIE_FILESYSTEM_DRIVES_HH
#define GENIE_FILESYSTEM_DRIVES_HH

// vfs
#include "vfs/node_fwd.hh"


namespace Genie
{
	
	void volume_flush_trigger( const vfs::node* that );
	void volume_eject_trigger( const vfs::node* that );
	void volume_mount_trigger( const vfs::node* that );
	void volume_unmount_trigger( const vfs::node* that );
	
}

#endif
