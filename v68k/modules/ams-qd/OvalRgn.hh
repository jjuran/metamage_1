/*
	OvalRgn.hh
	----------
*/

#ifndef OVALRGN_HH
#define OVALRGN_HH

struct MacRegion;

void OvalRgn( MacRegion** rgn, short width, short height );

void FramedOvalRgn( MacRegion**  rgn,
                    short        width,
                    short        height,
                    short        penWidth,
                    short        penHeight );

#endif
