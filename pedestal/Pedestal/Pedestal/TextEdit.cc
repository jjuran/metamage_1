/*	===========
 *	TextEdit.cc
 *	===========
 */

#include "Pedestal/TextEdit.hh"

// Standard C++
#include <algorithm>

// Mac OS
#include <Fonts.h>

// Nucleus
#include "Nucleus/NAssert.h"

// Nitrogen
#include "Nitrogen/Events.h"
#include "Nitrogen/TextEdit.h"

// Pedestal
#include "Pedestal/Application.hh"
#include "Pedestal/AutoKey.hh"
#include "Pedestal/Clipboard.hh"
#include "Pedestal/CustomTEClickLoop.hh"
#include "Pedestal/IncrementalSearch.hh"
#include "Pedestal/Quasimode.hh"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	
	static bool LeftOrRightArrowsKeyed()
	{
		N::GetKeys_Result keys = N::GetKeys();
		
		const UInt8 high7x = keys.keyMapByteArray[ 0x7b >> 3 ];
		
		const UInt8 leftRightArrowBits = 1 << (0x7b & 0x07) | 1 << (0x7c & 0x07);
		
		return high7x & leftRightArrowBits;
	}
	
	void TextEdit::Idle( const EventRecord& )
	{
		TEHandle hTE = Get();
		
		ASSERT( hTE != NULL );
		
		N::TEIdle( hTE );
		
		AdjustSleepForTimer( ::GetCaretTime() );
		
		if ( !LeftOrRightArrowsKeyed() )
		{
			ResetArrowKeyChordability();
		}
	}
	
	bool TextEdit::SetCursor( const EventRecord&  event,
	                          RgnHandle           mouseRgn )
	{
		N::SetCursor( N::GetCursor( N::iBeamCursor ) );
		
		return true;
	}
	
	void TextEdit::MouseDown( const EventRecord& event )
	{
		ASSERT( Get() != NULL );
		
		TEClickLoop_Scope scope( this );
		
		N::TEClick( N::GlobalToLocal( event.where ),
		            event.modifiers & shiftKey,
		            Get() );
		
		On_UserSelect();
	}
	
	
	static inline bool NewContentReplacesSelection()
	{
		// True in Mac HIG, false in The Humane Interface
		return false;
	}
	
	static inline bool KeyIsAllowedAgainstSelection( char c, TEHandle aTE )
	{
		if ( NewContentReplacesSelection() )
		{
			return true;
		}
		
		// Allow control keys always (backspace, arrows)
		// Allow content keys only when selection is empty (insertion point)
		return c < 0x20  ||  aTE[0]->selStart == aTE[0]->selEnd;
	}
	
	bool TextEdit::KeyDown( const EventRecord& event )
	{
		TEHandle hTE = Get();
		
		ASSERT( hTE != NULL );
		
		const char c = event.message & charCodeMask;
		
		if ( Try_IgnoreAutoKey( event ) )
		{
			// do nothing
		}
		else if ( Try_RepeatSearch( *this, event ) )
		{
			// already handled
		}
		else if ( Try_ArrowKeyChord( *this, c ) )
		{
			// already handled
		}
		else if ( !KeyIsAllowedAgainstSelection( c, hTE ) )
		{
			static UInt32 lastBeep = 0;
			
			if ( event.when - lastBeep > 45 )
			{
				lastBeep = event.when;
				
				N::SysBeep();
			}
			
			// do nothing
		}
		else
		{
			N::TEKey( c, hTE );
			
			On_UserEdit();
		}
		
		return true;
	}
	
	
	void TextEdit::Activate( bool activating )
	{
		TEHandle hTE = Get();
		
		ASSERT( hTE != NULL );
		
		if ( activating )
		{
			N::TEActivate( hTE );
		}
		else
		{
			N::TEDeactivate( hTE );
		}
		
		ResetArrowKeyChordability();
	}
	
	static void EraseBlankArea( TEHandle hTE )
	{
		TERec& te = **hTE;
		
		Rect viewRect = te.viewRect;
		
		// View height in pixels
		short viewHeight = viewRect.bottom - viewRect.top;
		
		// Text height in pixels
		// NOTE:  nLines counts displayable lines only, ignoring a trailing CR.
		int textHeight = te.nLines * te.lineHeight;
		
		int textBottom = te.destRect.top + textHeight;
		
		// If the bottom of the text doesn't reach the bottom of the viewing area,
		if ( textBottom < viewRect.bottom )
		{
			// then below the text is a blank space, which TEUpdate() ignores.
			// So we need to erase it ourselves.
			viewRect.top = textBottom;
			
			N::EraseRect( viewRect );
		}
	}
	
	void TextEdit::Draw( const Rect& bounds )
	{
		using Nucleus::Operators::operator!=;
		
		TEHandle hTE = Get();
		
		ASSERT( hTE != NULL );
		
		TERec& te = **hTE;
		
		if ( bounds != te.viewRect )
		{
			short dv = te.viewRect.top - te.destRect.top;
			
			te.viewRect = bounds;
			te.destRect = N::OffsetRect( bounds, 0, -dv );
		}
		
		N::TEUpdate( bounds, hTE );
		
		EraseBlankArea( hTE );
	}
	
	bool TextEdit::UserCommand( MenuItemCode code )
	{
		TEHandle hTE = Get();
		
		ASSERT( hTE != NULL );
		
		switch ( code )
		{
			// Edit
			case 'undo':
				//Undo();
				break;
			
			case 'cut ':
				Clipboard::TECut( hTE );
				
				On_UserEdit();
				
				break;
			
			case 'copy':
				Clipboard::TECopy( hTE );
				break;
			
			case 'past':  // kHICommandPaste
			case 'pste':
				Clipboard::TEPaste( hTE );
				
				On_UserEdit();
				
				break;
			
			case 'clea':
				N::TEDelete( hTE );
				
				On_UserEdit();
				
				break;
			
			case 'sall':  // kHICommandSelectAll
			case 'slct':
				Select( 0, 32767 );
				break;
			
			default:
				return false;
				break;
		}
		
		return true;
	}
	
	
	static TextSelection GetSelection( TEHandle hTE )
	{
		ASSERT( hTE != NULL );
		
		struct TextSelection result;
		
		const TERec& te = **hTE;
		
		result.start = te.selStart;
		result.end   = te.selEnd;
		
		return result;
	}
	
	TextSelection TextEdit::GetCurrentSelection() const
	{
		return GetSelection( Get() );
	}
	
	void TextEdit::Select( unsigned start, unsigned end )
	{
		ASSERT( Get() != NULL );
		
		N::TESetSelect( start, end, Get() );
		
		On_UserSelect();
	}
	
	static short TESearch( TEHandle              hTE,
	                       const std::string&    pattern,
	                       const TextSelection&  selection,
	                       bool                  backward,
	                       bool                  matchAtPosition )
	{
		const TERec& te = **hTE;
		
		return TextSearch( *te.hText,
		                   te.teLength,
		                   pattern,
		                   selection,
		                   backward,
		                   matchAtPosition );
	}
	
	int TextEdit::Search( const std::string&    pattern,
	                      const TextSelection&  selection,
	                      bool                  searchBackwards,
	                      bool                  matchAtPosition ) const
	{
		ASSERT( Get() != NULL );
		
		return TESearch( Get(),
		                 pattern,
		                 selection,
		                 searchBackwards,
		                 matchAtPosition );
	}
	
}

