// Nitrogen/TextEdit.cp
// --------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NITROGEN_TEXTEDIT_H
#include "Nitrogen/TextEdit.h"
#endif

// Standard C++
#include <algorithm>

// Nitrogen
#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif


namespace Nitrogen
{
	
	Nucleus::Owned< TEHandle > TENew( const Rect& destRect, const Rect& viewRect )
	{
		return Nucleus::Owned< TEHandle >::Seize( ::TENew( &destRect, &viewRect ) );
	}
	
	void TESetText( const void* text, std::size_t length, TEHandle hTE )
	{
		::TESetText( text, length, hTE );
	}
	
	void TESetText( const std::string& text, TEHandle hTE )
	{
		TESetText( text.data(), text.size(), hTE );
	}
	
	std::string TEGetText( TEHandle hTE )
	{
		std::size_t length = (**hTE).teLength;
		std::string text;
		text.resize( length );
		::Handle h = (**hTE).hText;
		std::copy( *h, *h + length, text.begin() );
		
		return text;
	}
	
	void TESetSelect( std::size_t selStart, std::size_t selEnd, TEHandle hTE )
	{
		::TESetSelect( selStart, selEnd, hTE );
	}
	
	void TEKey( char key, TEHandle hTE )
	{
		::TEKey( key, hTE );
	}
	
	void TEInsert( const void* text, std::size_t length, TEHandle hTE )
	{
		::TEInsert( text, length, hTE );
	}
	
	void TEInsert( const std::string& text, TEHandle hTE )
	{
		TEInsert( text.data(), text.size(), hTE );
	}
	
	void TEUpdate( const Rect& rUpdate, TEHandle hTE )
	{
		::TEUpdate( &rUpdate, hTE );
	}
	
	void TETextBox( const void* text, std::size_t length, const Rect& box, Justification just )
	{
		::TETextBox( text, length, &box, just );
	}
	
	void TETextBox( const std::string& text, const Rect& box, Justification just )
	{
		TETextBox( text.data(), text.size(), box, just );
	}
	
	void TEFromScrap()
	{
		ThrowOSStatus( ::TEFromScrap() );
	}
	
	void TEToScrap()
	{
		ThrowOSStatus( ::TEToScrap() );
	}
	
}

