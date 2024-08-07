/*
	Cursor.cc
	---------
*/

#include "Cursor.hh"

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// ams-common
#include "cursor_jump.hh"


pascal void InitCursor_patch()
{
	JInitCrsr();
}

pascal void SetCursor_patch( const Cursor* crsr )
{
	JSetCrsr( crsr );
}

pascal void HideCursor_patch()
{
	JHideCursor();
}

pascal void ShowCursor_patch()
{
	JShowCursor();
}

pascal void ObscureCursor_patch()
{
	JCrsrObscure();
}

pascal void ShieldCursor_patch( const Rect* rect, Point offset )
{
	JShieldCursor( rect, offset );
}
