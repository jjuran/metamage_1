/*
	decode.hh
	---------
*/

#ifndef DECODE_HH
#define DECODE_HH

// mac-types
#include "mac_types/VRefNum_DirID.hh"


typedef short FSIORefNum;
typedef short OSErr;

OSErr Decode( FSIORefNum input, const mac::types::VRefNum_DirID& destDir );

#endif
