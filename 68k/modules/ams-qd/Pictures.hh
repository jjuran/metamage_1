/*
	Pictures.hh
	-----------
*/

#ifndef PICTURES_HH
#define PICTURES_HH

struct Picture;
struct Rect;

pascal Picture** OpenPicture_patch( const Rect* frame );

pascal void ClosePicture_patch();

pascal void KillPicture_patch( Picture** pic );

pascal void DrawPicture_patch( Picture** pic, const Rect* dstRect );

#endif
