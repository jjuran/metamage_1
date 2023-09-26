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

asm
pascal void HideCursor_patch()
{
	LINK     A6,#0
	MOVEM.L  D1-D2/A0-A1,-(SP)
	
	/*
		Aside from the Resource Manager, Toolbox traps are allowed to clobber
		volatile registers, but Vanlandingham assumes that HideCursor() does
		not.  (Specifically, it relies on D1-D2/A0-A1 being preserved.)
		
		TODO:  Replace this workaround with an INIT-loading system that can
		install patches for individual applications.
	*/
	
	MOVEA.L  JHideCursor,A0
	JSR      (A0)
	
	MOVEM.L  (SP)+,D1-D2/A0-A1
	UNLK     A6
	RTS
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
