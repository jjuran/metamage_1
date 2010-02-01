/*
	
	Josh's Keys
	
	Joshua Juran
	
*/

// Mac OS
#include <Events.h>
#include <Resources.h>
#include <Sound.h>
#include <TextEdit.h>

// Standard C++
#include <algorithm>

// Silver
#include "Silver/Install.hh"
#include "Silver/Patch.hh"
#include "Silver/Procs.hh"
#include "Silver/Traps.hh"


#pragma exceptions off


namespace Ag = Silver;

static bool gExtendingSelection;
static short gSelectionAnchor, gSelectionExtent;


static const UInt32 kEitherShiftKey   = shiftKey   | rightShiftKey;
static const UInt32 kEitherOptionKey  = optionKey  | rightOptionKey;
static const UInt32 kEitherControlKey = controlKey | rightControlKey;

inline bool CharIsHorizontalArrow( char c )
{
	return (c & 0xFE) == 0x1C;
}

inline bool CharIsVerticalArrow( char c )
{
	return (c & 0xFE) == 0x1E;
}

inline bool CharIsForwardArrow( char c )
{
	return (c & 0xFD) == 0x1D;
}

inline bool CharIsArrow( char c )
{
	return (c & 0xFC) == 0x1C;
}

static bool CharIsWordChar( char c )
{
	if ( c == '_' )  return true;
	
	if ( c >= '0'  &&  c <= '9' )  return true;
	
	c |= ' ';
	
	return c >= 'a'  &&  c <= 'z';
}

static bool CharIsDelete( char c )
{
	return c == kBackspaceCharCode  ||  c == kDeleteCharCode;
}

static bool CharIsSpecialForCmdOrOption( char c)
{
	return CharIsDelete( c ) || CharIsArrow( c );
}

static inline UInt16 GetPartialKeyModifiers()
{
	// Credit to Lawrence D'Oliveiro for the low memory tip, from PsyScript.
	UInt16 lowMemModifiers = *reinterpret_cast< UInt16* >( 0x017A );
	
	UInt16 keyModifiers = (lowMemModifiers & 0x007F) << 9   // Shift/Alpha/Option/Control
	                    | (lowMemModifiers & 0x8000) >> 7;  // Command
	
	return keyModifiers;
}


namespace
{
	
	void TEActivate_Patch( TEHandle hTE, Ag::TEActivateProcPtr nextHandler )
	{
		nextHandler( hTE );
		
		gExtendingSelection = false;
	}
	
	void TEClick_Patch( Point pt, short extend, TEHandle hTE, Ag::TEClickProcPtr nextHandler )
	{
		nextHandler( pt, extend, hTE );
		
		gExtendingSelection = false;
	}
		
	void TEKey_Patch( short c, TEHandle hTE, Ag::TEKeyProcPtr nextHandler )
	{
		const UInt16 modifiers = GetPartialKeyModifiers();
		
		short selStart = hTE[0]->selStart;
		short selEnd   = hTE[0]->selEnd;
		
		bool emptySelection = selStart == selEnd;
		
		bool cmdKeyIsDown    = modifiers & cmdKey;
		bool shiftKeyIsDown  = modifiers & kEitherShiftKey;
		bool optionKeyIsDown = modifiers & kEitherOptionKey;
		
		bool forward = CharIsForwardArrow( c );
		
		bool deleting = CharIsDelete( c );
		
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
				if ( selEnd == hTE[0]->teLength )
				{
					return;
				}
				
				hTE[0]->selEnd += 1;
				
				c = kBackspaceCharCode;
			}
		}
		
		bool initializingSelection = CharIsArrow( c )  &&  (!gExtendingSelection || !shiftKeyIsDown);
		
		if ( initializingSelection )
		{
			// The selection anchor and extent must be initialized when we start or
			// finish extending the selection.
			gSelectionAnchor =  forward ? selStart : selEnd;
			gSelectionExtent = !forward ? selStart : selEnd;
		}
		
		gExtendingSelection = CharIsArrow( c ) && shiftKeyIsDown;
		
		if ( gExtendingSelection || CharIsVerticalArrow( c ) )
		{
			// Set our extent as the insertion point so we can use TextEdit's arrow
			// behavior.
			::TESetSelect( gSelectionExtent, gSelectionExtent, hTE );
		}
		
		if ( CharIsArrow( c )  &&  (cmdKeyIsDown || optionKeyIsDown) )
		{
			// Dereferencing a Handle here
			const char* begin = hTE[0]->hText[0];
			const char* end   = begin + hTE[0]->teLength;
			
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
							while ( --p > begin  &&  CharIsWordChar( p[-1] ) )  continue;
						break;
					
					case kRightArrowCharCode:
						if ( p < end )
							while ( ++p < end  &&  CharIsWordChar( p[0] ) )  continue;
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
		else if ( !gExtendingSelection  &&  CharIsHorizontalArrow( c )  &&  !emptySelection )
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
			// No special logic, just call the original TEKey().
			nextHandler( c, hTE );
			
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
	
	void TEInit_Patch( Ag::TEInitProcPtr nextHandler )
	{
		Handle hack = ::Get1NamedResource( 'Hack', "\p" "JoshsKeys" );
		
		if ( hack != NULL )
		{
			::ReleaseResource( hack );
		}
		else
		{
			Ag::TrapPatch< _TEActivate, TEActivate_Patch >::Install();
			Ag::TrapPatch< _TEClick,    TEClick_Patch    >::Install();
			Ag::TrapPatch< _TEKey,      TEKey_Patch      >::Install();
		}
		
		nextHandler();
	}
	
}


static OSErr Installer()
{
	gExtendingSelection = false;
	
	Ag::TrapPatch< _TEInit, TEInit_Patch >::Install();
	
	return noErr;
}

int main()
{
	return Ag::Install( Installer );
}

