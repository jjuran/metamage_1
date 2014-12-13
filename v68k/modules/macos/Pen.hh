/*
	Pen.hh
	------
*/

#ifndef PEN_HH
#define PEN_HH

struct Pattern;
struct PenState;
struct Point;

pascal void StdLine_patch( Point newPt );

pascal void HidePen_patch();
pascal void ShowPen_patch();

pascal void GetPen_patch( Point* pt );

pascal void GetPenState_patch(       PenState* state );
pascal void SetPenState_patch( const PenState* state );

pascal void PenSize_patch( short width, short height );
pascal void PenMode_patch( short mode );

pascal void PenPat_patch( const Pattern* pat );

pascal void PenNormal_patch();

pascal void MoveTo_patch( short h,  short v  );
pascal void Move_patch  ( short dh, short dv );
pascal void LineTo_patch( short h,  short v  );
pascal void Line_patch  ( short dh, short dv );

#endif
