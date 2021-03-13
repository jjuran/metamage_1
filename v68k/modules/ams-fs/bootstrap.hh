/*
	bootstrap.hh
	------------
*/

#ifndef BOOTSTRAP_HH
#define BOOTSTRAP_HH

// Standard
#include <stdint.h>


struct FCB;
struct HFileParam;
struct VCB;

typedef short OSErr;


void mount_virtual_bootstrap_volume();

const uint8_t* bootstrap_get_nth( VCB* vcb, short n );

const uint8_t* bootstrap_lookup( VCB* vcb, const uint8_t* name );

OSErr bootstrap_open_fork( short trap_word, FCB* fcb, const uint8_t* name );

OSErr bootstrap_GetFileInfo( HFileParam* pb, const uint8_t* name );

#endif
