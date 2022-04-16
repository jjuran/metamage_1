/*
	Arcs.hh
	-------
*/

#ifndef ARCS_HH
#define ARCS_HH

struct Rect;
struct Pattern;

pascal void StdArc_patch( signed char verb, const Rect* r, short a, short e );

pascal void EraseArc_patch( const Rect* rect, short start, short extent );
pascal void PaintArc_patch( const Rect* rect, short start, short extent );
pascal void InvertArc_patch( const Rect* rect, short start, short extent );
pascal void FrameArc_patch( const Rect* rect, short start, short extent );

pascal void FillArc_patch( const Rect*     rect,
                           short           start,
                           short           extent,
                           const Pattern*  pattern );

#endif
