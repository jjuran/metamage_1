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
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// mac-rsrc-utils
#include "mac_rsrc/get_string_from_resource.hh"

// mac-app-utils
#include "mac_app/CGAboutBox.hh"
#include "mac_app/QDAboutBox.hh"


#pragma exceptions off


#define STRLEN( s )  (sizeof "" s - 1)

#define STR_LEN( s )  "" s, (sizeof s - 1)


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

/*
	Avoid the need to link with libc for memmove()
	by defining our own version.
*/

static inline
void MemMove( void* dst, const void* src, size_t n )
{
	return BlockMoveData( src, dst, n );
}

#define MemCopy MemMove

static
bool get_About_box_data( OSType creator )
{
	static Str255 name;
	static Str255 vers;
	static Str255 copy;
	
	if ( Handle h = GetResource( creator, 0 ) )
	{
		GetResInfo( h, NULL, NULL, name );
		
		mac::rsrc::get_string_from_handle( copy, h );
	}
	
	mac::rsrc::get_vers_ShortVersionString( vers, 1 );
	
	if ( vers[ 0 ] <= 255 - STRLEN( "Version " ) )
	{
		MemMove( vers + 1 + STRLEN( "Version " ), vers + 1, vers[ 0 ] );
		MemCopy( vers + 1, STR_LEN( "Version " ) );
		
		vers[ 0 ] += STRLEN( "Version " );
	}
	
	gAboutBoxAppName = name;
	gAboutBoxAppVers = vers;
	gAboutBoxAppCopy = copy;
	
	return true;
}

void show_About_box( OSType creator )
{
	static bool init = ! TARGET_RT_MAC_MACHO  &&  get_About_box_data( creator );
	
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
