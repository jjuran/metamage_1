/*	==========
 *	TextEdit.h
 *	==========
 */

#ifndef NITROGEN_TEXTEDIT_H
#define NITROGEN_TEXTEDIT_H

// Universal Interfaces
#ifndef __TEXTEDIT__
#include <TextEdit.h>
#endif

// Standard C++
#include <string>

// Nitrogen
#ifndef NITROGEN_OWNED_H
#include "Nitrogen/Owned.h"
#endif
#ifndef NITROGEN_SELECTORTYPE_H
#include "Nitrogen/SelectorType.h"
#endif


namespace Nitrogen
{
	
	struct Justification_Tag  {};
	typedef SelectorType< Justification_Tag, short, teFlushDefault > Justification;
	
	inline Justification TEFlushDefault()  { return Justification::Make( teFlushDefault ); }
	
	using ::TEHandle;
	
	template <>
	struct Disposer< TEHandle >
	{
		void operator()( TEHandle teH ) const
		{
			::TEDispose( teH );
		}
	};
	
	Owned< TEHandle > TENew( const Rect& destRect, const Rect& viewRect );
	
	inline Owned< TEHandle > TENew( const Rect& rect )
	{
		return TENew( rect, rect );
	}
	
	inline void TEDispose( Owned< TEHandle > )  {}
	
	void TESetText( const void* text, std::size_t length, TEHandle hTE );
	void TESetText( const std::string& text, TEHandle hTE );
	
	std::string TEGetText( TEHandle hTE );
	
	using ::TEIdle;
	
	void TESetSelect( std::size_t selStart, std::size_t selEnd, TEHandle hTE );
	
	using ::TEActivate;
	using ::TEDeactivate;
	
	void TEKey( char key, TEHandle hTE );
	
	using ::TECut;
	using ::TECopy;
	using ::TEPaste;
	using ::TEDelete;
	
	void TEInsert( const void* text, std::size_t length, TEHandle hTE );
	void TEInsert( const std::string& text, TEHandle hTE );
	
	void TEUpdate( const Rect& rUpdate, TEHandle hTE );
	
	void TETextBox( const void* text, std::size_t length, const Rect& box, Justification just = TEFlushDefault() );
	void TETextBox( const std::string& text, const Rect& box, Justification just = TEFlushDefault() );
	
	using ::TEPinScroll;
	using ::TEAutoView;
	using ::TECalText;
	
	// 1650
	inline void TEClick( Point pt, bool extendFlag, TEHandle hTE )  { ::TEClick( pt, extendFlag, hTE ); }
	
	// 1913
	void TEFromScrap();
	
	// 1925
	void TEToScrap();
	
}

#endif

