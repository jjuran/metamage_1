/*
	bootstrap.cc
	------------
*/

#include "bootstrap.hh"

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif

// ams-common
#include "callouts.hh"

// ams-fs
#include "mount.hh"


void mount_virtual_bootstrap_volume()
{
	static VCB virtual_bootstrap_volume_VCB;
	
	VCB* vcb = &virtual_bootstrap_volume_VCB;
	
	fast_memset( vcb, '\0', sizeof (VCB) );
	
	vcb->vcbSigWord = 'Ix';
	vcb->vcbFSID    = 'Ix';
	
	#define VOLNAME  "\p" "Bootstrap"
	
	fast_memcpy( vcb->vcbVN, VOLNAME, sizeof VOLNAME );
	
	#undef VOLNAME
	
	mount_VCB( vcb );
}
