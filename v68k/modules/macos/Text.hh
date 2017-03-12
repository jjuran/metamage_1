/*
	Text.hh
	-------
*/

#ifndef TEXT_HH
#define TEXT_HH

struct FontInfo;
struct Point;

pascal void DrawChar_patch( short c );

pascal void DrawString_patch( const unsigned char* s );

pascal void DrawText_patch( const char* buffer, short offset, short n );

pascal short CharWidth_patch( short c );

pascal short StringWidth_patch( const unsigned char* s );

pascal short TextWidth_patch( const char* buffer, short offset, short n );

pascal void GetFontInfo_patch( FontInfo* info );

pascal short StdTxMeas_patch( short        n,
                              const char*  p,
                              Point*       numer,
                              Point*       denom,
                              FontInfo*    info );

pascal void StdText_patch( short n, const char* p, Point numer, Point denom );

#endif
