/*	====================
 *	IncrementalSearch.cc
 *	====================
 */

#include "Pedestal/IncrementalSearch.hh"

// Standard C/C++
#include <cctype>

// Debug
#include "debug/assert.hh"

// Nitrogen
#include "Nitrogen/Events.h"
#include "Nitrogen/Resources.h"
#include "Nitrogen/Sound.h"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	
	
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
		
		N::GetKeys_Result keys = N::GetKeys();
		
		return std::memcmp( keys.keyMapByteArray, desiredKeys, sizeof desiredKeys ) == 0;
	}
	
	
	void IncrementalSearchEditor::AugmentSelection()
	{
		TextSelection current = GetCurrentSelection();
		TextSelection prior   = GetPriorSelection();
		
		if ( prior.start < 0 )
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
	
	bool Try_ArrowKeyChord( IncrementalSearchEditor& editor, char c )
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
			
			if ( *text != *pattern  &&  std::tolower( *text ) != *pattern )
			{
				return false;
			}
		}
		
		return true;
	}
	
	short TextSearch( const char*           text,
	                  std::size_t           text_length,
	                  const std::string&    pattern,
	                  const TextSelection&  selection,
	                  bool                  backward,
	                  bool                  matchAtPosition )
	{
		short increment = backward ? -1 : 1;
		
		int position = pattern.size() == 1 ? backward ? selection.start - pattern.size()
		                                              : selection.end
		             : selection.start + !matchAtPosition * increment;
		
		int maxPosition = text_length - pattern.size();
		
		int limit = backward ? -1 : maxPosition + 1;
		
		while ( position != limit )
		{
			if ( pattern_match( text + position, pattern.data(), pattern.size() ) )
			{
				return position;
			}
			
			position += increment;
		}
		
		return -1;
	}
	
	
	static std::string gLastSearchPattern;
	
	const std::string& GetLastSearchPattern()
	{
		return gLastSearchPattern;
	}
	
	static char GetTranslatedKeyFromEvent( const EventRecord& event, UInt16 ignoredModifierMask )
	{
		static UInt32 state = 0;
		
		Handle kchr = GetResource( 'KCHR', 0 );
		
		N::ResError();
		
		ASSERT(  kchr != NULL );
		ASSERT( *kchr != NULL );
		
		UInt16 keyCode = (event.message & keyCodeMask) >> 8;
		
		keyCode |= event.modifiers & (0xff00 - ignoredModifierMask);
		
		UInt32 key = KeyTranslate( *kchr, keyCode, &state );
		
		const char c = key & 0xff;
		
		return c;
	}
	
	
	IncrementalSearchQuasimode::IncrementalSearchQuasimode( IncrementalSearchEditor&  editor,
	                                                        bool                      backward )
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
					
					N::SysBeep( 30 );
				}
			}
		}
		else if ( c == 0x08 )
		{
			if ( itsPattern.empty() )
			{
				N::SysBeep();
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
			
			short match = itsView.Search( itsPattern, selection, itSearchesBackward, true );
			
			if ( match == -1 )
			{
				itsView.SetCurrentSelection( itsSavedSelection );
				
				N::SysBeep();
			}
			else
			{
				itsView.Select( match, match + itsPattern.size() );
			}
		}
		
		return true;
	}
	
	bool Try_RepeatSearch( IncrementalSearchEditor& editor, const EventRecord& event )
	{
		const char c = event.message & charCodeMask;
		
		const UInt16 bothShiftKeys = shiftKey | rightShiftKey;
		
		const UInt16 shifted = event.modifiers & bothShiftKeys;
		
		if ( c == ' '  &&  shifted  &&  (event.modifiers & cmdKey) )
		{
			if ( shifted == bothShiftKeys )
			{
				N::SysBeep();
				
				return true;
			}
			
			const bool backward = shifted == shiftKey;
			
			TextSelection selection = editor.GetCurrentSelection();
			
			short match = editor.Search( GetLastSearchPattern(), selection, backward, false );
			
			if ( match == -1 )
			{
				N::SysBeep();
			}
			else
			{
				editor.Select( match, match + GetLastSearchPattern().length() );
			}
			
			return true;
		}
		
		return false;
	}
	
}

