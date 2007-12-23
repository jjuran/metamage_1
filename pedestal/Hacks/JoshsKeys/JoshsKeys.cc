/*
	
	Josh's Keys
	
	Joshua Juran
	
*/

// Universal Interfaces
#include <Events.h>
#include <Resources.h>
#include <Sound.h>
#include <TextEdit.h>

// Standard C/C++
#include <cstring>

// Standard C++
#include <algorithm>

// Silver
#include "Silver/Install.hh"
#include "Silver/Patch.hh"
#include "Silver/Procs.hh"
#include "Silver/Traps.hh"


#pragma exceptions off


namespace Ag = Silver;

static EventRecord gLastEvent;
static bool gExtendingSelection;
static short gSelectionAnchor, gSelectionExtent;


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

namespace
{
	
	short PatchedGetNextEvent( EventMask eventMask, EventRecord* theEvent, Ag::GetNextEventProcPtr nextHandler )
	{
		short result = nextHandler( eventMask, theEvent );
		
		gLastEvent = *theEvent;
		
		return result;
	}
	
	void PatchedTEActivate( TEHandle hTE, Ag::TEActivateProcPtr nextHandler )
	{
		nextHandler( hTE );
		
		gExtendingSelection = false;
	}
	
	void PatchedTEClick( Point pt, short extend, TEHandle hTE, Ag::TEClickProcPtr nextHandler )
	{
		nextHandler( pt, extend, hTE );
		
		gExtendingSelection = false;
	}
		
	void PatchedTEKey( short c, TEHandle hTE, Ag::TEKeyProcPtr nextHandler )
	{
		short selStart = hTE[0]->selStart;
		short selEnd   = hTE[0]->selEnd;
		
		bool emptySelection = selStart == selEnd;
		
		bool cmdKeyIsDown    = gLastEvent.modifiers & cmdKey;
		bool shiftKeyIsDown  = gLastEvent.modifiers & shiftKey;
		bool optionKeyIsDown = gLastEvent.modifiers & optionKey;
		
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
	
}


static OSErr Installer()
{
	gExtendingSelection = false;
	
	Ag::TrapPatch< _GetNextEvent, PatchedGetNextEvent >::Install();
	Ag::TrapPatch< _TEActivate,   PatchedTEActivate   >::Install();
	Ag::TrapPatch< _TEClick,      PatchedTEClick      >::Install();
	Ag::TrapPatch< _TEKey,        PatchedTEKey        >::Install();
	
	return noErr;
}

int main()
{
	return Ag::Install( Installer );
}

