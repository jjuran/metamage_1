// Nitrogen/Fonts.cc
// -----------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#include "Nitrogen/Fonts.hh"

// missing-macos
#ifdef MAC_OS_X_VERSION_10_7
#ifndef MISSING_FONTS_H
#include "missing/Fonts.h"
#endif
#endif


namespace Nitrogen
{
	
#if ! __LP64__
	
	FontID GetFNum( ConstStr255Param name )
	{
		short fontNum;
		::GetFNum( name, &fontNum );
		return FontID( fontNum );
	}
	
#else
	
	void dummy()
	{
	}
	
#endif  // #if ! __LP64__
	
}
