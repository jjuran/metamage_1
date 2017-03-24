/*
	RoundRects.hh
	-------------
*/

#ifndef ROUNDRECTS_HH
#define ROUNDRECTS_HH

struct Rect;
struct Pattern;

pascal void StdRRect_patch( signed char  verb,
                            const Rect*  r,
                            short        ovalWidth,
                            short        ovalHeight );

pascal void EraseRoundRect_patch( const Rect* rect, short w, short h );
pascal void PaintRoundRect_patch( const Rect* rect, short w, short h );
pascal void InverRoundRect_patch( const Rect* rect, short w, short h );
pascal void FrameRoundRect_patch( const Rect* rect, short w, short h );

pascal void FillRoundRect_patch( const Rect*     rect,
                                 short           ovalWidth,
                                 short           ovalHeight,
                                 const Pattern*  pattern );

#endif
