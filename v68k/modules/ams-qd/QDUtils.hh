/*
	QDUtils.hh
	----------
*/

#ifndef QDUTILS_HH
#define QDUTILS_HH

pascal unsigned char BitTst_patch( char* addr, long bit );
pascal void          BitSet_patch( char* addr, long bit );
pascal void          BitClr_patch( char* addr, long bit );

pascal short Random_patch();

pascal long BitAnd_patch( long a, long b );

pascal void StuffHex_patch( char* dst, const unsigned char* srcHex );

pascal struct Pattern** GetPattern_patch( short id );
pascal struct Cursor**  GetCursor_patch ( short id );
pascal struct Picture** GetPicture_patch( short id );

#endif
