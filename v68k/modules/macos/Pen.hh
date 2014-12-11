/*
	Pen.hh
	------
*/

#ifndef PEN_HH
#define PEN_HH

struct PenState;

pascal void GetPenState_patch(       PenState* state );
pascal void SetPenState_patch( const PenState* state );

#endif
