/*
	TextEdit.hh
	-----------
*/

#ifndef TEXTEDIT_HH
#define TEXTEDIT_HH

struct Rect;

pascal void TEInit_patch();

pascal void TETextBox_patch( const char* p, long n, const Rect* r, short just );

#endif
