/*
	QDUtils.hh
	----------
*/

#ifndef QDUTILS_HH
#define QDUTILS_HH

typedef long Fixed;

struct MacRegion;
struct Point;
struct Rect;

typedef MacRegion** RgnHandle;

pascal void UnpackBits_patch( char** src, char** dst, short dstBytes );

pascal unsigned char BitTst_patch( char* addr, long bit );
pascal void          BitSet_patch( char* addr, long bit );
pascal void          BitClr_patch( char* addr, long bit );

pascal short Random_patch();

pascal long BitAnd_patch( long a, long b );
pascal long BitNot_patch( long x );

pascal unsigned long BitShift_patch( unsigned long x, short n );

pascal void StuffHex_patch( char* dst, const unsigned char* srcHex );

pascal void MapPt_patch( Point* pt, const Rect* src, const Rect* dst );
pascal void MapRect_patch( Rect* r, const Rect* src, const Rect* dst );
pascal void MapRgn_patch( RgnHandle rgn, const Rect* src, const Rect* dst );

pascal struct Pattern** GetPattern_patch( short id );
pascal struct Cursor**  GetCursor_patch ( short id );
pascal struct Picture** GetPicture_patch( short id );

pascal long DeltaPoint_patch( Point a, Point b );

pascal Fixed SlopeFromAngle_patch( short angle );
pascal short AngleFromSlope_patch( Fixed slope );

pascal long PinRect_patch( const Rect* rect, Point pt );

#endif
