/*
	about_box.cc
	------------
*/

#include "mac_app/about_box.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif

// mac-app-utils
#include "mac_app/CGAboutBox.hh"
#include "mac_app/QDAboutBox.hh"


#pragma exceptions off


namespace mac {
namespace app {

#if ! __LP64__

#define WindowList (*(WindowRef*) 0x09D6)

static inline
WindowRef get_WindowList()
{
	return ACCESSOR_CALLS_ARE_FUNCTIONS ? GetWindowList() : WindowList;
}

static inline
bool is_About_box( WindowRef window )
{
	return GetWindowKind( window ) == kAboutWindowKind;
}

static
WindowRef find_About_box()
{
	WindowRef window = get_WindowList();
	
	for ( ;  window != NULL;  window = GetNextWindow( window ) )
	{
		if ( is_About_box( window ) )
		{
			return window;
		}
	}
	
	return window;  // NULL
}

void show_About_box()
{
	WindowRef gAboutBox = find_About_box();
	
	if ( gAboutBox )
	{
		SelectWindow( gAboutBox );
		return;
	}
	
	if ( TARGET_RT_MAC_MACHO )
	{
		gAboutBox = CGAboutBox_make();
	}
	else
	{
		gAboutBox = QDAboutBox_make();
	}
	
	SetWindowKind( gAboutBox, kAboutWindowKind );
}

bool close_About_box( WindowRef window )
{
	const bool is_About = is_About_box( window );
	
	if ( is_About )
	{
		if ( TARGET_RT_MAC_MACHO )
		{
			CGAboutBox_close( window );
		}
		else
		{
			QDAboutBox_close( window );
		}
	}
	
	return is_About;
}

#else  // #if ! __LP64__

int dummy;

#endif

}
}
