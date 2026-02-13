/*
	mount.hh
	--------
*/

#ifndef MOUNT_HH
#define MOUNT_HH

struct VCB;

typedef unsigned char Boolean;

short mount_VCB( VCB* vcb, Boolean first = false );

void try_to_mount( const char* name );

#endif
