/*
	remotefs.hh
	-----------
*/

#ifndef REMOTEFS_HH
#define REMOTEFS_HH

struct HFileParam;
struct VCB;

typedef unsigned char Byte;
typedef short         OSErr;


const Byte* remotefs_lookup( VCB* vcb, const Byte* name );

OSErr remotefs_GetFileInfo( int in, int out, HFileParam* pb, const Byte* name );

#endif
