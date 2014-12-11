/*
	Pen.hh
	------
*/

#ifndef PEN_HH
#define PEN_HH

struct PenState;
struct Point;

pascal void GetPen_patch( Point* pt );

pascal void GetPenState_patch(       PenState* state );
pascal void SetPenState_patch( const PenState* state );

#endif
