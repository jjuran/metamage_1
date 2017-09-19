/*
	QDUtils.hh
	----------
*/

#ifndef QDUTILS_HH
#define QDUTILS_HH

pascal short Random_patch();

pascal void StuffHex_patch( char* dst, const unsigned char* srcHex );

pascal struct Pattern** GetPattern_patch( short id );
pascal struct Cursor**  GetCursor_patch ( short id );

#endif
