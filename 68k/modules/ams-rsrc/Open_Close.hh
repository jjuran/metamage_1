/*
	Open_Close.hh
	-------------
*/

#ifndef OPENCLOSE_HH
#define OPENCLOSE_HH


typedef unsigned char Byte;
typedef   signed char SInt8;


pascal short OpenResFile_patch( const Byte* name );

pascal void CloseResFile_patch( short refnum );

pascal short OpenRFPerm_patch( const Byte* name, short vRefNum, SInt8 perm );

#endif
