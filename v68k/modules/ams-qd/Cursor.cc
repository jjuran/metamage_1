/*
	Cursor.cc
	---------
*/

#include "Cursor.hh"

// ams-common
#include "QDGlobals.hh"
#include "cursor_jump.hh"


pascal void InitCursor_patch()
{
	QDGlobals& qd = get_QDGlobals();
	
	JSetCrsr( &qd.arrow );
	
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
