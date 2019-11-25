/*
	Polygons.hh
	-----------
*/

#ifndef POLYGONS_HH
#define POLYGONS_HH

struct MacPolygon;
struct MacRegion;
struct Pattern;

void PolyRgn( MacRegion** rgn, MacPolygon** poly );

pascal void FramePoly_patch( MacPolygon** poly );
pascal void PaintPoly_patch( MacPolygon** poly );
pascal void ErasePoly_patch( MacPolygon** poly );
pascal void InvertPoly_patch( MacPolygon** poly );
pascal void FillPoly_patch( MacPolygon** poly, const Pattern* pattern );

pascal void StdPoly_patch( signed char verb, MacPolygon** poly );

#endif
