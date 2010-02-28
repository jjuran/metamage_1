/*
	WindowList_contains.cc
	----------------------
	
	Copyright 2009, Joshua Juran
*/

#include "Genie/Utilities/WindowList_contains.hh"

// plus
#include "plus/contains.hh"

// Nitrogen
#include "Nitrogen/MacWindows.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	
	bool WindowList_contains( WindowRef window )
	{
		return window != NULL  &&  plus::contains( N::WindowList(), window );
	}
	
}

