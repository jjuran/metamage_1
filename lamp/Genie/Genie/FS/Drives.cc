/*	=========
 *	Drives.cc
 *	=========
 */

#include "Genie/FS/Drives.hh"

// Nitrogen
#ifndef MAC_FILES_FUNCTIONS_FLUSHVOL_HH
#include "Mac/Files/Functions/FlushVol.hh"
#endif
#ifndef MAC_FILES_FUNCTIONS_UNMOUNTVOL_HH
#include "Mac/Files/Functions/UnmountVol.hh"
#endif

// ClassicToolbox
#if !TARGET_API_MAC_CARBON
#ifndef CLASSICTOOLBOX_FILES_HH
#include "ClassicToolbox/Files.hh"
#endif
#endif

// Nitrogen
#ifndef NITROGEN_FILES_HH
#include "Nitrogen/Files.hh"
#endif

// Genie
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/Trigger.hh"


namespace Genie
{
	
	void volume_flush_trigger( const FSTree* node )
	{
		trigger_extra& extra = *(trigger_extra*) node->extra();
		
		Mac::FSVolumeRefNum vRefNum = Mac::FSVolumeRefNum( extra.data );
		
		Mac::FlushVol( vRefNum );
	}
	
#if !TARGET_API_MAC_CARBON
	
	void volume_eject_trigger( const FSTree* node )
	{
		trigger_extra& extra = *(trigger_extra*) node->extra();
		
		Mac::FSVolumeRefNum vRefNum = Mac::FSVolumeRefNum( extra.data );
		
		Nitrogen::Eject( vRefNum );
	}
	
	void volume_mount_trigger( const FSTree* node )
	{
		trigger_extra& extra = *(trigger_extra*) node->extra();
		
		Mac::FSVolumeRefNum vRefNum = Mac::FSVolumeRefNum( extra.data );
		
		Nitrogen::MountVol( vRefNum );
	}
	
#endif
	
	void volume_unmount_trigger( const FSTree* node )
	{
		trigger_extra& extra = *(trigger_extra*) node->extra();
		
		Mac::FSVolumeRefNum vRefNum = Mac::FSVolumeRefNum( extra.data );
		
		Mac::UnmountVol( vRefNum );
	}
	
}

