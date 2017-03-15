/*
	StrUtils.hh
	-----------
*/

#ifndef STRUTILS_HH
#define STRUTILS_HH

pascal unsigned char** NewString_patch( const unsigned char* s );

pascal void SetString_patch( unsigned char** h, const unsigned char* s );

pascal unsigned char** GetString_patch( short resID );

#endif
