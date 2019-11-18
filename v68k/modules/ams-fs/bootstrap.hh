/*
	bootstrap.hh
	------------
*/

#ifndef BOOTSTRAP_HH
#define BOOTSTRAP_HH

// Standard
#include <stdint.h>


struct FCB;

typedef short OSErr;


void mount_virtual_bootstrap_volume();

OSErr bootstrap_open_fork( short trap_word, FCB* fcb, const uint8_t* name );

#endif
