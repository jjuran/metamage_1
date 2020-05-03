/*
	TextEdit.hh
	-----------
*/

#ifndef TEXTEDIT_HH
#define TEXTEDIT_HH

struct Point;
struct Rect;
struct TERec;

pascal void TEInit_patch();

pascal TERec** TENew_patch( const Rect* destRect, const Rect* viewRect );

pascal void TEDispose_patch( TERec** hTE );

pascal void TESetText_patch( const char* p, long n, TERec** hTE );

pascal void TEIdle_patch( TERec** hTE );

pascal void TEClick_patch( Point pt, char extend, TERec** hTE );

pascal void TESetSelect_patch( long selStart, long selEnd, TERec** hTE );

pascal void TEActivate_patch( TERec** hTE );
pascal void TEDeactivate_patch( TERec** hTE );

pascal void TEKey_patch( short c, TERec** hTE );

pascal void TECut_patch( TERec** hTE );
pascal void TECopy_patch( TERec** hTE );
pascal void TEPaste_patch( TERec** hTE );

pascal void TEDelete_patch( TERec** hTE );

pascal void TEInsert_patch( const char* text, long length, TERec** hTE );

pascal void TESetJust_patch( short just, TERec** hTE );

pascal void TEUpdate_patch( const Rect* updateRect, TERec** hTE );

pascal void TETextBox_patch( const char* p, long n, const Rect* r, short just );

pascal void TEScroll_patch( short dh, short dv, TERec** hTE );

pascal void TECalText_patch( TERec** hTE );

pascal TERec** TEStyleNew_patch( const Rect* destRect, const Rect* viewRect );

#endif
