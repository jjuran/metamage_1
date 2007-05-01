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

static bool IsHArrow( char c )
{
	return c == kLeftArrowCharCode  ||  c == kRightArrowCharCode;
}

static bool DoSpecial( char c, TEHandle hTE )
{
	bool hArrow = IsHArrow( c );
	bool shift = ( gLastEvent.modifiers & shiftKey );
	//bool option = (gLastEvent.modifiers & optionKey);
	bool forward = ( c == kRightArrowCharCode );
	
	short selStart = (**hTE).selStart;
	short selEnd   = (**hTE).selEnd;
	
	if ( hArrow && shift )
	{
		if ( !gExtendingSelection )
		{
			gExtendingSelection = true;
			gSelectionAnchor =  forward ? selStart : selEnd;
			gSelectionExtent = !forward ? selStart : selEnd;
		}
		
		gSelectionExtent += ( forward ? 1 : -1 );
		gSelectionExtent = std::max< short >( 0, gSelectionExtent );
		gSelectionExtent = std::min( (**hTE).teLength, gSelectionExtent );
		
		selStart = std::min( gSelectionAnchor, gSelectionExtent );
		selEnd   = std::max( gSelectionAnchor, gSelectionExtent );
		
		::TESetSelect( selStart, selEnd, hTE );
		
		return true;
	}
	else
	{
		gExtendingSelection = false;
		
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
			
			return true;
		}
	}
	
	return false;
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

static pascal void PatchedTEKey( short c, TEHandle hTE )
{
	Ag::MyA4 a4;
	
	if ( !DoSpecial( c, hTE ) )
	{
		gTEKeyPatch.Original()( c, hTE );
	}
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

