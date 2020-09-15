/*
	Window.hh
	---------
*/

#ifndef TESTEDIT_WINDOW_HH
#define TESTEDIT_WINDOW_HH

// plus
#include "plus/string_fwd.hh"

// nucleus
#ifndef NUCLEUS_OWNED_HH
#include "nucleus/owned.hh"
#endif

// Nitrogen
#ifndef MAC_WINDOWS_TYPES_WINDOWREF_HH
#include "Mac/Windows/Types/WindowRef.hh"
#endif


namespace TestEdit
{
	
	class ScrollFrame;
	
	typedef ScrollFrame View;
	
	nucleus::owned< WindowRef >
	//
	NewWindow( ConstStr255Param title = "\p" "TestEdit" );
	
	void SetText( ScrollFrame& view, const plus::string& text );
	
}

#endif
