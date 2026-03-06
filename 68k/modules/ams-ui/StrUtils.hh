/*
	StrUtils.hh
	-----------
*/

#ifndef STRUTILS_HH
#define STRUTILS_HH


typedef unsigned char    Byte;
typedef unsigned char**  StringHandle;


pascal StringHandle NewString_patch( const Byte* s );

pascal void SetString_patch( StringHandle h, const Byte* s );

#endif
