/*
	appfs.hh
	--------
*/

#ifndef APPFS_HH
#define APPFS_HH

struct FCB;

typedef short OSErr;

extern int appfs_fd;

void load_app_data( FCB* fcb );

OSErr save_app_data( const FCB* fcb );

#endif
