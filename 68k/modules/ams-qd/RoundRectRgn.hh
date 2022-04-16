/*
	RoundRectRgn.hh
	---------------
*/

#ifndef ROUNDRECTRGN_HH
#define ROUNDRECTRGN_HH

struct MacRegion;

void RoundRectRgn( MacRegion**  rgn,
                   short        width,
                   short        height,
                   short        ovalWidth,
                   short        ovalHeight );

void FramedRoundRectRgn( MacRegion**  rgn,
                         short        width,
                         short        height,
                         short        ovalWidth,
                         short        ovalHeight,
                         short        penWidth,
                         short        penHeight );

#endif
