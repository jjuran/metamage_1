// Nitrogen/TextUtils.cp
// ---------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2005 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NITROGEN_TEXTUTILS_H
#include "Nitrogen/TextUtils.h"
#endif

// Standard C++
#include <algorithm>

// Nitrogen
#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif


namespace Nitrogen
{
	
	Nucleus::Owned< StringHandle > NewString( ConstStr255Param string )
	{
		StringHandle result = ::NewString( string );
		
		MemError();
		
		return Nucleus::Owned< StringHandle >::Seize( result );
	}
	
	void SetString( StringHandle string, ConstStr255Param newStr )
	{
		::SetString( string, newStr );
		
		MemError();
	}
	
	StringHandle GetString( ResID stringID )
	{
		StringHandle result = ::GetString( stringID );
		
		ResError();
		
		return result;
	}
	
	Str255 GetIndString( ResID resID, UInt16 index )
	{
		::Str255 str;
		::GetIndString( str, resID, index );
		
		ResError();
		
		return str;
	}
	
	std::string p2cstrcpy( ConstStr255Param src )
	{
		return Nucleus::Convert< std::string >( src );
	}
	
}

