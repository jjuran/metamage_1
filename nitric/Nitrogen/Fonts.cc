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


namespace Nitrogen
{
	
	FontID GetFNum( ConstStr255Param name )
	{
		short fontNum;
		::GetFNum( name, &fontNum );
		return FontID( fontNum );
	}
	
	FontID GetFNum( const std::string& name )
	{
		return GetFNum( Str255( name ) );
	}
	
}

