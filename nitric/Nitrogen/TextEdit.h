// Nitrogen/TextEdit.h
// -------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004-2006 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NITROGEN_TEXTEDIT_H
#define NITROGEN_TEXTEDIT_H

// Universal Interfaces
#ifndef __TEXTEDIT__
#include <TextEdit.h>
#endif

// Standard C++
#include <string>

// Nucleus
#ifndef NUCLEUS_SELECTOR_H
#include "Nucleus/Selector.h"
#endif

// Nitrogen
#ifndef NITROGEN_MACMEMORY_H
#include "Nitrogen/MacMemory.h"
#endif
#ifndef NITROGEN_QUICKDRAW_H
#include "Nitrogen/QuickDraw.h"
#endif


namespace Nitrogen
{
	
	typedef Nucleus::Selector< class Justification_Tag, short >::Type Justification;
	
	using ::TEHandle;
	
  }

namespace Nucleus
  {
	template <>
	struct Disposer< Nitrogen::TEHandle >: public std::unary_function< Nitrogen::TEHandle, void >
	{
		void operator()( Nitrogen::TEHandle teH ) const
		{
			::TEDispose( teH );
		}
	};
  }

namespace Nitrogen
  {
	
	Nucleus::Owned< TEHandle > TENew( const Rect& destRect, const Rect& viewRect );
	
	inline Nucleus::Owned< TEHandle > TENew( const Rect& rect )
	{
		return TENew( rect, rect );
	}
	
	inline void TEDispose( Nucleus::Owned< TEHandle > )  {}
	
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
	
	void TETextBox( const void* text, std::size_t length, const Rect& box, Justification just = Justification( teFlushDefault ) );
	void TETextBox( const std::string& text, const Rect& box, Justification just = Justification( teFlushDefault ) );
	
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

