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
	
	
	static bool gExtendingSelection = false;
	
	static short gSelectionAnchor, gSelectionExtent;
	
	
	TextEdit::TextEdit() : itIsActive()
	{
	}
	
	static bool LeftOrRightArrowsKeyed()
	{
		N::GetKeys_Result keys = N::GetKeys();
		
		const UInt8 high7x = keys.keyMapByteArray[ 0x7b >> 3 ];
		
		const UInt8 leftRightArrowBits = 1 << (0x7b & 0x07) | 1 << (0x7c & 0x07);
		
		return high7x & leftRightArrowBits;
	}
	
	void TextEdit::Idle( const EventRecord& )
	{
		if ( !itIsActive )
		{
			return;
		}
		
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
	
	bool TextEdit::MouseDown( const EventRecord& event )
	{
		ASSERT( Get() != NULL );
		
		TEClickLoop_Scope scope( this );
		
		N::TEClick( N::GlobalToLocal( event.where ),
		            event.modifiers & shiftKey,
		            Get() );
		
		gExtendingSelection = false;
		
		On_UserSelect();
		
		return true;
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
	
	static inline bool char_is_horizontal_arrow( char c )
	{
		return (c & 0xFE) == 0x1C;
	}

	static inline bool char_is_vertical_arrow( char c )
	{
		return (c & 0xFE) == 0x1E;
	}
	
	static inline bool char_is_forward_arrow( char c )
	{
		return (c & 0xFD) == 0x1D;
	}
	
	static inline bool char_is_arrow( char c )
	{
		return (c & 0xFC) == 0x1C;
	}
	
	static inline bool char_is_delete( char c )
	{
		return c == kBackspaceCharCode  ||  c == kDeleteCharCode;
	}
	
	static bool char_is_word_char( char c )
	{
		if ( c == '_' )  return true;
		
		if ( c >= '0'  &&  c <= '9' )  return true;
		
		c |= 'a' - 'A';
		
		return c >= 'a'  &&  c <= 'z';
	}
	
	static void TEKeyEvent( const EventRecord& event, TEHandle hTE )
	{
		const UInt32 kEitherShiftKey   = shiftKey   | rightShiftKey;
		const UInt32 kEitherOptionKey  = optionKey  | rightOptionKey;
		const UInt32 kEitherControlKey = controlKey | rightControlKey;
		
		char c =  event.message & charCodeMask;
		
		bool cmdKeyIsDown    = event.modifiers & cmdKey;
		bool shiftKeyIsDown  = event.modifiers & kEitherShiftKey;
		bool optionKeyIsDown = event.modifiers & kEitherOptionKey;
		
		// Dereferencing hTE
		TERec& te = **hTE;
		
		short selStart = te.selStart;
		short selEnd   = te.selEnd;
		
		const bool emptySelection = selStart == selEnd;
		
		const bool forward = char_is_forward_arrow( c );
		
		const bool deleting = char_is_delete( c );
		
		if ( deleting )
		{
			// Shift-Backspace is Forward Delete
			if ( shiftKeyIsDown )
			{
				c = kDeleteCharCode;
				
				shiftKeyIsDown = false;
			}
			
			if ( (cmdKeyIsDown || optionKeyIsDown) && emptySelection )
			{
				// For Option- or Cmd-Delete, pretend we're selecting first
				c = c == kBackspaceCharCode ? kLeftArrowCharCode
											: kRightArrowCharCode;
				
				shiftKeyIsDown = true;
			}
			else if ( !emptySelection )
			{
				// On a selection, Cmd- and Option-Delete are the same as Delete.
				cmdKeyIsDown    = false;
				optionKeyIsDown = false;
				
				// On a selection, Forward Delete is the same as Backspace.
				c = kBackspaceCharCode;
			}
			else if ( c == kDeleteCharCode )
			{
				if ( selEnd == te.teLength )
				{
					return;
				}
				
				te.selEnd += 1;
				
				c = kBackspaceCharCode;
			}
		}
		
		const bool initializingSelection = char_is_arrow( c )  &&  (!gExtendingSelection || !shiftKeyIsDown);
		
		if ( initializingSelection )
		{
			// The selection anchor and extent must be initialized when we start or
			// finish extending the selection.
			gSelectionAnchor =  forward ? selStart : selEnd;
			gSelectionExtent = !forward ? selStart : selEnd;
		}
		
		gExtendingSelection = char_is_arrow( c ) && shiftKeyIsDown;
		
		if ( gExtendingSelection || char_is_vertical_arrow( c ) )
		{
			// Set our extent as the insertion point so we can use TextEdit's arrow
			// behavior.
			::TESetSelect( gSelectionExtent, gSelectionExtent, hTE );
		}
		
		if ( char_is_arrow( c )  &&  (cmdKeyIsDown || optionKeyIsDown) )
		{
			// Dereferencing hTE
			const TERec& te = **hTE;
			
			// Dereferencing te.hText
			const char* begin = te.hText[0];
			const char* end   = begin + te.teLength;
			
			const char* p = begin + gSelectionExtent;
			
			if ( cmdKeyIsDown )
			{
				switch ( c )
				{
					case kLeftArrowCharCode:
						while ( p > begin  &&  p[-1] != '\r' )  --p;
						break;
					
					case kRightArrowCharCode:
						while ( p < end  &&  p[0] != '\r' )  ++p;
						break;
					
					case kUpArrowCharCode:
						p = begin;
						break;
					
					case kDownArrowCharCode:
						p = end;
						break;
					
				}
			}
			else if ( optionKeyIsDown )
			{
				switch ( c )
				{
					case kLeftArrowCharCode:
						if ( p > begin )
							while ( --p > begin  &&  char_is_word_char( p[-1] ) )  continue;
						break;
					
					case kRightArrowCharCode:
						if ( p < end )
							while ( ++p < end  &&  char_is_word_char( p[0] ) )  continue;
						break;
					
					case kUpArrowCharCode:
					case kDownArrowCharCode:
						::SysBeep( 30 );  // May move memory
						break;
				}
			}
			
			gSelectionExtent = p - begin;
			
			if ( !gExtendingSelection )
			{
				// Update the real insertion point
				::TESetSelect( gSelectionExtent, gSelectionExtent, hTE );
			}
		}
		else if ( !gExtendingSelection  &&  char_is_horizontal_arrow( c )  &&  !emptySelection )
		{
			// Workaround TextEdit's bug where left- or right-arrow places the
			// insertion point past the selection instead of at the edge of it.
			
			if ( c == kRightArrowCharCode )
			{
				selStart = selEnd;
			}
			else
			{
				selEnd = selStart;
			}
			
			// Update the real insertion point
			::TESetSelect( selStart, selEnd, hTE );
		}
		else
		{
			N::TEKey( c, hTE );
			
			// Update the extent
			gSelectionExtent = hTE[0]->selStart;
		}
		
		if ( gExtendingSelection )
		{
			selStart = std::min( gSelectionAnchor, gSelectionExtent );
			selEnd   = std::max( gSelectionAnchor, gSelectionExtent );
			
			// Update the real selection
			::TESetSelect( selStart, selEnd, hTE );
			
			if ( deleting )
			{
				::TEDelete( hTE );
			}
		}
	}
	
	bool TextEdit::Preprocess_Key( const EventRecord& event )
	{
		return    Try_IgnoreAutoKey( event )
		       || Try_RepeatSearch( *this, event )
		       || event.what == keyDown && Try_ArrowKeyChord( *this, event.message & charCodeMask );
	}
	
	bool TextEdit::Process_Key( const EventRecord& event )
	{
		TEHandle hTE = Get();
		
		ASSERT( hTE != NULL );
		
		const char c =  event.message & charCodeMask;
		
		if ( !KeyIsAllowedAgainstSelection( c, hTE ) )
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
			TEKeyEvent( event, hTE );
			
			On_UserEdit();
		}
		
		return true;
	}
	
	bool TextEdit::KeyDown( const EventRecord& event )
	{
		const char c   =  event.message & charCodeMask;
		const char key = (event.message & keyCodeMask) >> 8;
		
		if ( Preprocess_Key( event ) )
		{
			// already handled
		}
		else if ( c == kEnterCharCode  &&  key >= 0x30 )
		{
			On_EnterKey();
		}
		else if ( c <= 0x1b  &&  c != 0x08  &&  (c != 0x0d || IsSingular()) )
		{
			// Don't process control chars except arrow keys, BS, and maybe CR
			return false;
		}
		else if ( Process_Key( event ) )
		{
			Postprocess_Key( event );
		}
		else
		{
			return false;
		}
		
		return true;
	}
	
	boost::shared_ptr< Quasimode >
	//
	TextEdit::EnterShiftSpaceQuasimode( const EventRecord& event )
	{
		const bool backward = event.modifiers & shiftKey;
		
		boost::shared_ptr< Quasimode > mode( new IncrementalSearchQuasimode( *this, backward ) );
		
		return mode;
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
		
		gExtendingSelection = false;
		
		ResetArrowKeyChordability();
		
		itIsActive = activating;
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
	
	void TextEdit::Draw( const Rect& bounds, bool erasing )
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
			
			N::TECalText( hTE );
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
	
	
	static short CountLinesForEditing( const TERec& te )
	{
		short teLength = te.teLength;
		
		// An empty buffer counts as one line
		if ( teLength == 0 )
		{
			return 1;
		}
		
		short    nLines = te.nLines;
		::Handle hText  = te.hText;
		
		// Find the last character in the buffer
		char c = ( *hText )[ teLength - 1 ];
		
		// If the last char is a carriage return, add an extra line
		if ( c == '\r' )
		{
			++nLines;
		}
		
		return nLines;
	}
	
	unsigned GetTextEditingHeight( const TERec& te )
	{
		return CountLinesForEditing( te ) * te.lineHeight;
	}
	
}

