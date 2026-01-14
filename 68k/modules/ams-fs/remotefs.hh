/*
	remotefs.hh
	-----------
*/

#ifndef REMOTEFS_HH
#define REMOTEFS_HH

// plus
#include "plus/var_string_fwd.hh"


struct HFileParam;
struct VCB;

typedef unsigned char Byte;
typedef short         OSErr;

const Byte* remotefs_get_nth( VCB* vcb, int n, plus::var_string& cache );

const Byte* remotefs_lookup( VCB* vcb, const Byte* name );

OSErr remotefs_GetFileInfo( HFileParam* pb, const Byte* name );

#endif
