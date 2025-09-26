/*
	preferences.cc
	--------------
*/

#include "preferences.hh"

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif

// ams-common
#include "callouts.hh"

// ams-fs
#include "mount.hh"


#pragma exceptions off


int prefs_fd;

short prefs_vRefNum;

void mount_virtual_preferences_volume()
{
	static VCB virtual_preferences_volume_VCB;
	
	VCB* vcb = &virtual_preferences_volume_VCB;
	
	vcb->vcbSigWord = 0x3e1a;  // "meta", as in metadata
	vcb->vcbFSID    = 0x3e1a;
	
	// 16 MiB free should be sufficient for now
	vcb->vcbAlBlkSiz = 4096;
	vcb->vcbFreeBks  = 4096;
	
	vcb->vcbDRefNum = prefs_fd;
	
	#define VOLNAME  "\p" "Preferences"
	
	fast_memcpy( vcb->vcbVN, VOLNAME, sizeof VOLNAME );
	
	#undef VOLNAME
	
	prefs_vRefNum = mount_VCB( vcb );
}
