/*
	TextEdit.hh
	-----------
*/

#ifndef TEXTEDIT_HH
#define TEXTEDIT_HH

struct Rect;
struct TERec;

pascal void TEInit_patch();

pascal void TEIdle_patch( TERec** hTE );

pascal void TETextBox_patch( const char* p, long n, const Rect* r, short just );

#endif
