/*
	Pictures.hh
	-----------
*/

#ifndef PICTURES_HH
#define PICTURES_HH

typedef char** Handle;

struct Picture;
struct Rect;

pascal Picture** OpenPicture_patch( const Rect* frame );

pascal void ClosePicture_patch();

pascal void PicComment_patch( short kind, short dataSize, Handle data );

pascal void KillPicture_patch( Picture** pic );

pascal void DrawPicture_patch( Picture** pic, const Rect* dstRect );

#endif
