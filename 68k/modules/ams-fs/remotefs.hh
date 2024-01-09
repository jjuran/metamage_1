/*
	remotefs.hh
	-----------
*/

#ifndef REMOTEFS_HH
#define REMOTEFS_HH

struct VCB;

typedef unsigned char Byte;


const Byte* remotefs_lookup( VCB* vcb, const Byte* name );

#endif
