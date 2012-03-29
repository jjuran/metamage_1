/*	=========
 *	Drives.hh
 *	=========
 */

#ifndef GENIE_FILESYSTEM_DRIVES_HH
#define GENIE_FILESYSTEM_DRIVES_HH


namespace Genie
{
	
	class FSTree;
	
	void volume_flush_trigger( const FSTree* node );
	void volume_eject_trigger( const FSTree* node );
	void volume_mount_trigger( const FSTree* node );
	void volume_unmount_trigger( const FSTree* node );
	
}

#endif

