/*
	Pictures.hh
	-----------
*/

#ifndef PICTURES_HH
#define PICTURES_HH

struct Picture;
struct Rect;

pascal void DrawPicture_patch( Picture** pic, const Rect* dstRect );

#endif
