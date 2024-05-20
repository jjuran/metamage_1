/*
	Polygons.hh
	-----------
*/

#ifndef POLYGONS_HH
#define POLYGONS_HH

struct MacPolygon;
struct MacRegion;
struct Pattern;
struct Point;

typedef MacPolygon Polygon;
typedef MacRegion  Region;

typedef Polygon** PolyHandle;
typedef Region**  RgnHandle;

void add_polygon_point( Point pt, PolyHandle poly );

void PolyRgn( RgnHandle rgn, PolyHandle poly );

pascal void FramePoly_patch( PolyHandle poly );
pascal void PaintPoly_patch( PolyHandle poly );
pascal void ErasePoly_patch( PolyHandle poly );
pascal void InvertPoly_patch( PolyHandle poly );
pascal void FillPoly_patch( PolyHandle poly, const Pattern* pattern );

pascal void StdPoly_patch( signed char verb, PolyHandle poly );

#endif
