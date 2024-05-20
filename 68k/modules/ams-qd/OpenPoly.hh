/*
	OpenPoly.hh
	-----------
*/

#ifndef OPENPOLY_HH
#define OPENPOLY_HH

struct GrafPort;
struct MacPolygon;

typedef MacPolygon** PolyHandle;

pascal PolyHandle OpenPoly_patch();

pascal void ClosePoly_patch();

pascal void KillPoly_patch( PolyHandle poly );

pascal void OffsetPoly_patch( PolyHandle poly, short dh, short dv );

#endif
