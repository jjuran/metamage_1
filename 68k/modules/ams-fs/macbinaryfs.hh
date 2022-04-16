/*
	macbinaryfs.hh
	--------------
*/

#ifndef MACBINARYFS_HH
#define MACBINARYFS_HH

// macbinary
#include "macbinary.hh"


struct FCB;
struct HFileParam;
struct VCB;

typedef short OSErr;


const macbinary::header* MacBinary_get_nth( VCB* vcb, short n );

const macbinary::header* MacBinary_lookup( VCB* vcb, const uint8_t* name );

OSErr MacBinary_Close( FCB* fcb );

OSErr MacBinary_open_fork( short trap_word, FCB* fcb, const macbinary::hdr* h );

OSErr MacBinary_GetFileInfo( HFileParam* pb, const macbinary::hdr* h );

#endif
