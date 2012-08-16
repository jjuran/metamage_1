/*
	WindowList_contains.hh
	----------------------
	
	Copyright 2009, Joshua Juran
*/

#ifndef GENIE_UTILITIES_WINDOWLISTCONTAINS_HH
#define GENIE_UTILITIES_WINDOWLISTCONTAINS_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif


namespace Genie
{
	
	bool WindowList_contains( WindowRef window );
	
}

#endif

