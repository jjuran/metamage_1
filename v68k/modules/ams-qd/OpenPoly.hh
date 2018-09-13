/*
	OpenPoly.hh
	-----------
*/

#ifndef OPENPOLY_HH
#define OPENPOLY_HH

struct MacPolygon;
struct Point;

pascal MacPolygon** OpenPoly_patch();

void add_polygon_point( Point pt );

pascal void ClosePoly_patch();

pascal void KillPoly_patch( MacPolygon** poly );

pascal void OffsetPoly_patch( MacPolygon** poly, short dh, short dv );

#endif
