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
#include "Silver/PatchUtils.hh"
#include "Silver/Patches.hh"


namespace Ag = Silver;

static Ag::PatchApplied< Ag::GetNextEventPatch > gGetNextEventPatch;
static Ag::PatchApplied< Ag::TEActivatePatch   > gTEActivatePatch;
static Ag::PatchApplied< Ag::TEClickPatch      > gTEClickPatch;
static Ag::PatchApplied< Ag::TEKeyPatch        > gTEKeyPatch;

static EventRecord gLastEvent;
static bool gExtendingSelection;
static short gSelectionAnchor, gSelectionExtent;


static pascal short PatchedGetNextEvent( EventMask eventMask, EventRecord* theEvent )
{
	Ag::MyA4 a4;
	
	short result = gGetNextEventPatch.Original()( eventMask, theEvent );
	
	gLastEvent = *theEvent;
	
	return result;
}

static bool CharIsHorizontalArrow( char c )
{
	return c == kLeftArrowCharCode  ||  c == kRightArrowCharCode;
}

static bool CharIsVerticalArrow( char c )
{
	return c == kUpArrowCharCode  ||  c == kDownArrowCharCode;
}

static bool CharIsArrow( char c )
{
	return CharIsHorizontalArrow( c ) || CharIsVerticalArrow( c );
}

static pascal void PatchedTEKey( short c, TEHandle hTE )
{
	bool hArrow = CharIsHorizontalArrow( c );
	bool shift = ( gLastEvent.modifiers & shiftKey );
	//bool option = (gLastEvent.modifiers & optionKey);
	bool forward = ( c == kRightArrowCharCode );
	
	short selStart = hTE[0]->selStart;
	short selEnd   = hTE[0]->selEnd;
	
	if ( hArrow && shift )
	{
		// Shift-key behavior is only modified for horizontal arrows so far
		
		if ( !gExtendingSelection )
		{
			// We've just started using Shift to extend the selection.
			// Set the selection anchor and extent.
			gExtendingSelection = true;
			gSelectionAnchor =  forward ? selStart : selEnd;
			gSelectionExtent = !forward ? selStart : selEnd;
		}
		
		// Modify the selection based on which arrow key was pressed.
		gSelectionExtent += ( forward ? 1 : -1 );
		gSelectionExtent = std::max< short >( 0, gSelectionExtent );
		gSelectionExtent = std::min( hTE[0]->teLength, gSelectionExtent );
		
		selStart = std::min( gSelectionAnchor, gSelectionExtent );
		selEnd   = std::max( gSelectionAnchor, gSelectionExtent );
		
		::TESetSelect( selStart, selEnd, hTE );
		
		return;
	}
	else
	{
		gExtendingSelection = false;
		
		// Fix the case of hitting left- or right-arrow with a selection
		if ( hArrow  &&  selStart != selEnd )
		{
			if ( forward )
			{
				selStart = selEnd;
			}
			else
			{
				selEnd = selStart;
			}
			
			::TESetSelect( selStart, selEnd, hTE );
			
			return;
		}
	}
	
	gTEKeyPatch.Original()( c, hTE );
}

static pascal void PatchedTEActivate( TEHandle hTE )
{
	Ag::MyA4 a4;
	
	gTEActivatePatch.Original()( hTE );
	
	gExtendingSelection = false;
}

static pascal void PatchedTEClick( Point pt, short extend, TEHandle hTE )
{
	Ag::MyA4 a4;
	
	gTEClickPatch.Original()( pt, extend, hTE );
	
	gExtendingSelection = false;
}


static bool Install()
{
	bool locked = Ag::LoadAndLock();
	
	if ( !locked )
	{
		return false;
	}
	
	Ag::MyA4 a4;
	
	gExtendingSelection = false;
	
	gGetNextEventPatch = Ag::GetNextEventPatch( PatchedGetNextEvent );
	gTEActivatePatch   = Ag::TEActivatePatch  ( PatchedTEActivate   );
	gTEClickPatch      = Ag::TEClickPatch     ( PatchedTEClick      );
	gTEKeyPatch        = Ag::TEKeyPatch       ( PatchedTEKey        );
	
	return true;
}

void main()
{
	bool installed = Install();
}

