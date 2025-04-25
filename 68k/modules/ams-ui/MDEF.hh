/*
	MDEF.hh
	-------
*/

#ifndef MDEF_HH
#define MDEF_HH


struct MenuInfo;
struct Point;
struct Rect;

typedef MenuInfo** MenuRef;

pascal
void MDEF_0( short msg, MenuRef menu, Rect* r, Point pt, short* item );

#endif
