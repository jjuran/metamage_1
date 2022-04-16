/*
	mount.hh
	--------
*/

#ifndef MOUNT_HH
#define MOUNT_HH

struct VCB;

short mount_VCB( VCB* vcb );

void try_to_mount( const char* name );

#endif
