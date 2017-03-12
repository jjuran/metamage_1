/*
	Text.hh
	-------
*/

#ifndef TEXT_HH
#define TEXT_HH

struct FontInfo;
struct Point;

pascal void GetFontInfo_patch( FontInfo* info );

pascal short StdTxMeas_patch( short        n,
                              const char*  p,
                              Point*       numer,
                              Point*       denom,
                              FontInfo*    info );

pascal void StdText_patch( short n, const char* p, Point numer, Point denom );

#endif
