/*
	Points.hh
	---------
*/

#ifndef POINTS_HH
#define POINTS_HH

struct Point;

pascal void AddPt_patch( Point src, Point* dst );
pascal void SubPt_patch( Point src, Point* dst );

pascal void SetPt_patch( Point* pt, short h, short v );

pascal unsigned char EqualPt_patch( Point a, Point b );

#endif
