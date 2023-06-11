/*	====================
 *	IncrementalSearch.cc
 *	====================
 */

#include "Pedestal/IncrementalSearch.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __EVENTS__
#include <Events.h>
#endif
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// Standard C++
#include <vector>

// iota
#include "iota/char_types.hh"

// mac-sys-utils
#include "mac_sys/beep.hh"

// Debug
#include "debug/assert.hh"

// plus
#include "plus/var_string.hh"

// Pedestal
#include "Pedestal/TextEdit.hh"


namespace Pedestal
{
	
	static inline bool CharIsHorizontalArrow( char c )
	{
		return (c & 0xFE) == 0x1C;
	}
	
	static bool LeftAndRightArrowsKeyed()
	{
		KeyMapByteArray desiredKeys = { 0,  //  0 -  7
		                                0,  //  8 -  f
		                                0,  // 10 - 17
		                                0,  // 18 - 1f
		                                
		                                0,  // 20 - 27
		                                0,  // 28 - 2f
		                                0,  // 30 - 37
		                                0,  // 38 - 3f
		                                
		                                0,  // 40 - 47
		                                0,  // 48 - 4f
		                                0,  // 50 - 57
		                                0,  // 58 - 5f
		                                
		                                0,  // 60 - 67
		                                0,  // 68 - 6f
		                                0,  // 70 - 77
		                                1 << (0x7b & 0x07) | 1 << (0x7c & 0x07) };
		
		KeyMap keymap;
		
		GetKeys( keymap );
		
		return memcmp( keymap, desiredKeys, sizeof desiredKeys ) == 0;
	}
	
	
	void TextEdit::AugmentSelection()
	{
		TextSelection current = GetCurrentSelection();
		TextSelection prior   = GetPriorSelection();
		
		if ( !prior.defined() )
		{
			return;
		}
		
		Select( std::min( current.start, prior.start ),
		        std::max( current.end,   prior.end   ) );
	}
	
	
	static bool gArrowKeyMayBeChorded = false;
	
	
	void ResetArrowKeyChordability()
	{
		gArrowKeyMayBeChorded = false;
	}
	
	bool Try_ArrowKeyChord( TextEdit& editor, char c )
	{
		if ( !CharIsHorizontalArrow( c ) )
		{
			return false;
		}
		
		static bool selectionModified;
		
		static TextSelection savedSelection;
		
		if ( !gArrowKeyMayBeChorded )
		{
			// This is an independent arrow key, pressed by itself
			// (although the user could press the other one to complete the
			// chord, and this may have already happened).
			gArrowKeyMayBeChorded = true;
			
			// If both arrow keys are already down, then a chord occurs
			// (to be processed on the next key-down), so don't change the
			// selection in that case.
			selectionModified = !LeftAndRightArrowsKeyed();
			
			if ( selectionModified )
			{
				// Save the selection (which will be changed by the arrow key).
				// If this turns out to be a chord, we'll restore it.
				savedSelection = editor.GetCurrentSelection();
				
				return false;  // let the editor apply the arrow key
			}
		}
		else if ( LeftAndRightArrowsKeyed() )
		{
			// gArrowKeyMayBeChorded is true, so we're the second arrow key
			// to go down, and both arrow keys are still down.  Hit it!
			
			if ( selectionModified )
			{
				// Restore selection undone by previous arrow key
				editor.SetCurrentSelection( savedSelection );
			}
			
			editor.AugmentSelection();
		}
		
		return true;
	}
	
	
	static bool pattern_match( const char* text, const char* pattern, std::size_t length )
	{
		for ( const char* end = pattern + length;  pattern != end;  ++pattern,
		                                                            ++text )
		{
			// A lower-case pattern char can match an upper-case text char.
			
			if ( *text != *pattern  &&  iota::to_lower( *text ) != *pattern )
			{
				return false;
			}
		}
		
		return true;
	}
	
	short TextSearch( const char*           text,
	                  std::size_t           text_length,
	                  const char*           pattern,
	                  std::size_t           pattern_length,
	                  const TextSelection&  selection,
	                  bool                  backward,
	                  bool                  matchAtPosition )
	{
		short increment = backward ? -1 : 1;
		
		int position = pattern_length == 1 ? backward ? selection.start - pattern_length
		                                              : selection.end
		             : selection.start + !matchAtPosition * increment;
		
		int maxPosition = text_length - pattern_length;
		
		int limit = backward ? -1 : maxPosition + 1;
		
		if ( !backward  &&  position >= limit )
		{
			return -1;
		}
		
		while ( position != limit )
		{
			if ( pattern_match( text + position, pattern, pattern_length ) )
			{
				return position;
			}
			
			position += increment;
		}
		
		return -1;
	}
	
	
	static plus::string gLastSearchPattern;
	
	const plus::string& GetLastSearchPattern()
	{
		return gLastSearchPattern;
	}
	
	static char GetTranslatedKeyFromEvent( const EventRecord& event, UInt16 ignoredModifierMask )
	{
		static UInt32 state = 0;
		
		Handle kchr = GetResource( 'KCHR', 0 );
		
		if ( kchr == NULL )
		{
			return '\0';
		}
		
		ASSERT(  kchr != NULL );
		ASSERT( *kchr != NULL );
		
		UInt16 keyCode = UInt8( event.message >> 8 );
		
		keyCode |= event.modifiers & (0xff00 - ignoredModifierMask);
		
		UInt32 key = KeyTranslate( *kchr, keyCode, &state );
		
		const char c = key;
		
		return c;
	}
	
	
	class IncrementalSearchQuasimode : public Quasimode
	{
		private:
			TextEdit&                     itsView;
			bool                          itSearchesBackward;
			unsigned short                itsModifierMask;
			TextSelection                 itsSavedSelection;
			std::vector< TextSelection >  itsMatches;
			plus::var_string              itsPattern;
		
		public:
			IncrementalSearchQuasimode( TextEdit& editor, bool backward );
			
			~IncrementalSearchQuasimode();
			
			bool KeyDown( const EventRecord& event );
	};
	
	
	IncrementalSearchQuasimode::IncrementalSearchQuasimode( TextEdit&  editor,
	                                                        bool       backward )
	:
		itsView           ( editor   ),
		itSearchesBackward( backward ),
		itsModifierMask   ( backward ? shiftKey : rightShiftKey ),
		itsSavedSelection ( editor.GetCurrentSelection() )
	{
		itsView.BeginQuasimode();
	}
	
	IncrementalSearchQuasimode::~IncrementalSearchQuasimode()
	{
		itsView.EndQuasimode();
		
		if ( itsView.GetCurrentSelection() != itsSavedSelection )
		{
			gLastSearchPattern = itsPattern;
			
			itsView.SetPriorSelection( itsSavedSelection );
		}
	}
	
	bool IncrementalSearchQuasimode::KeyDown( const EventRecord& event )
	{
		if ( event.what == autoKey )
		{
			return true;  // eat auto-repeat keys
		}
		
		const UInt16 ignoredModifierMask = itsModifierMask;
		
		const char c = GetTranslatedKeyFromEvent( event, ignoredModifierMask );
		
		if ( ! c )
		{
			// No 'KCHR' resource?  Oh well, eat the event.
			return true;
		}
		
		if ( CharIsHorizontalArrow( c ) )
		{
			const bool goingToSearchBackward = c == kLeftArrowCharCode;
			
			if ( itSearchesBackward != goingToSearchBackward )
			{
				itSearchesBackward = goingToSearchBackward;
				
				if ( !itsPattern.empty() )
				{
					// FIXME:  Rerun the search in the other direction
					itsPattern.clear();
					itsMatches.clear();
					
					itsView.SetCurrentSelection( itsSavedSelection );
					
					mac::sys::beep();
				}
			}
		}
		else if ( c == 0x08 )
		{
			if ( itsPattern.empty() )
			{
				mac::sys::beep();
			}
			else
			{
				TextSelection match = itsMatches.back();
				
				itsMatches.pop_back();
				
				itsPattern.resize( itsPattern.size() - 1 );  // pop_back() isn't standard
				
				itsView.SetCurrentSelection( match );
			}
		}
		else
		{
			itsPattern += c;
			
			TextSelection selection = itsView.GetCurrentSelection();
			
			itsMatches.push_back( selection );
			
			short match = itsView.Search( itsPattern.data(), itsPattern.size(), selection, itSearchesBackward, true );
			
			if ( match == -1 )
			{
				itsView.SetCurrentSelection( itsSavedSelection );
				
				mac::sys::beep();
			}
			else
			{
				itsView.Select( match, match + itsPattern.size() );
			}
		}
		
		return true;
	}
	
	boost::intrusive_ptr< Quasimode >
	//
	New_IncrementalSearchQuasimode( TextEdit& editor, bool backward )
	{
		return boost::intrusive_ptr< Quasimode >( new IncrementalSearchQuasimode( editor, backward ) );
	}
	
	
	bool Try_RepeatSearch( TextEdit& editor, const EventRecord& event )
	{
		const char c = event.message;
		
		const UInt16 bothShiftKeys = shiftKey | rightShiftKey;
		
		const UInt16 shifted = event.modifiers & bothShiftKeys;
		
		if ( c == ' '  &&  shifted  &&  (event.modifiers & cmdKey) )
		{
			if ( shifted == bothShiftKeys )
			{
				mac::sys::beep();
				
				return true;
			}
			
			const bool backward = shifted == shiftKey;
			
			TextSelection selection = editor.GetCurrentSelection();
			
			const plus::string& last_pattern = GetLastSearchPattern();
			
			short match = editor.Search( last_pattern.data(), last_pattern.size(), selection, backward, false );
			
			if ( match == -1 )
			{
				mac::sys::beep();
			}
			else
			{
				editor.Select( match, match + last_pattern.length() );
			}
			
			return true;
		}
		
		return false;
	}
	
}
