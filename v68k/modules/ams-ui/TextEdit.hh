/*
	TextEdit.hh
	-----------
*/

#ifndef TEXTEDIT_HH
#define TEXTEDIT_HH

struct Rect;
struct TERec;

pascal void TEInit_patch();

pascal TERec** TENew_patch( const Rect* destRect, const Rect* viewRect );

pascal void TEDispose_patch( TERec** hTE );

pascal void TEIdle_patch( TERec** hTE );

pascal void TEActivate_patch( TERec** hTE );
pascal void TEDeactivate_patch( TERec** hTE );

pascal void TETextBox_patch( const char* p, long n, const Rect* r, short just );

#endif
