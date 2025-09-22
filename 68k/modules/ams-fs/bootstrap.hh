/*
	bootstrap.hh
	------------
*/

#ifndef BOOTSTRAP_HH
#define BOOTSTRAP_HH


struct FCB;
struct HFileParam;
struct VCB;

typedef unsigned char  Byte;
typedef short          OSErr;


void mount_virtual_bootstrap_volume();

const Byte* bootstrap_get_nth( VCB* vcb, short n );

OSErr bootstrap_open_fork( short trap_word, FCB* fcb, const Byte* name );

OSErr bootstrap_GetFileInfo( HFileParam* pb, const Byte* name );

#endif
