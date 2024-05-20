/*
	OpenPoly.hh
	-----------
*/

#ifndef OPENPOLY_HH
#define OPENPOLY_HH

struct GrafPort;
struct MacPolygon;
struct Point;

typedef MacPolygon** PolyHandle;

pascal PolyHandle OpenPoly_patch();

void add_polygon_point( Point pt, PolyHandle poly );

pascal void ClosePoly_patch();

pascal void KillPoly_patch( PolyHandle poly );

pascal void OffsetPoly_patch( PolyHandle poly, short dh, short dv );

#endif
