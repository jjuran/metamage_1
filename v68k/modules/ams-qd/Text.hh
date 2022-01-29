/*
	Text.hh
	-------
*/

#ifndef TEXT_HH
#define TEXT_HH

struct FontInfo;
struct Point;

pascal void TextFont_patch( short font );
pascal void TextFace_patch( short face );
pascal void TextMode_patch( short mode );
pascal void TextSize_patch( short size );

pascal void SpaceExtra_patch( long extra );  // Fixed

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

pascal void MeasureText_patch( short count, const char* p, short* locs );

#endif
