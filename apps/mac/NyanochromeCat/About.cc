/*
	About.cc
	--------
*/

#include "About.hh"

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
#ifndef __TOOLUTILS__
#include <ToolUtils.h>
#endif
#ifndef __TEXTEDIT__
#include <TextEdit.h>
#endif

// Standard C
#include <string.h>

// missing-macos
#ifdef MAC_OS_X_VERSION_10_7
#ifndef MISSING_QUICKDRAW_H
#include "missing/Quickdraw.h"
#endif
#ifndef MISSING_QUICKDRAWTEXT_H
#include "missing/QuickdrawText.h"
#endif
#endif

// more-libc
#include "more/string.h"

// mac-glue-utils
#include "mac_glue/Memory.hh"

// mac-sys-utils
#include "mac_sys/gestalt.hh"

// mac-rsrc-utils
#include "mac_rsrc/get_string_from_resource.hh"

// mac-qd-utils
#include "mac_qd/plot_icon_id.hh"
#include "mac_qd/globals/screenBits.hh"

// mac-app-utils
#include "mac_app/new_window.hh"


#pragma exceptions off


#define STRLEN( s )  (sizeof "" s - 1)

#define STR_LEN( s )  "" s, (sizeof s - 1)


enum
{
	kAboutBoxIconEdgeLength       = 32,
	
	kAboutBoxIconWidth            = kAboutBoxIconEdgeLength,
	kAboutBoxTextHorizontalMargin = 24,
	
	kAboutBoxTopMargin            =  8,
	kAboutBoxIconHeight           = kAboutBoxIconEdgeLength,
	kAboutBoxIconTopMargin        = kAboutBoxTopMargin + 12,
	kAboutBoxIconToTextGap        = 24,  // horizontal offset
	kAboutBoxAppNameHeight        = 19,
	kAboutBoxInterTextGap         =  8,
	kAboutBoxDetailHeight         = 12,
	kAboutBoxInterSectionGap      = 16,
	kAboutBoxBottomMargin         = 20,
	
	kAboutBoxWidth = 276,
	
	kAboutBoxHeight = kAboutBoxTopMargin
	                + kAboutBoxAppNameHeight
	                + kAboutBoxInterTextGap
	                + kAboutBoxDetailHeight
	                + kAboutBoxInterTextGap
	                + kAboutBoxDetailHeight
	                + kAboutBoxInterSectionGap
	                + kAboutBoxDetailHeight
	                + kAboutBoxBottomMargin,
};

static Str255 title;
static Str255 name;
static Str255 vers;
static Str255 copy;
static Str255 more;

static
bool get_data()
{
	const OSType creator = 'NYAN';
	
	GetIndString( title, 128, 1 );
	GetIndString( more,  128, 2 );
	
	if ( Handle h = GetResource( creator, 0 ) )
	{
		GetResInfo( h, NULL, NULL, name );
		
		mac::rsrc::get_string_from_handle( copy, h );
	}
	
	mac::rsrc::get_vers_ShortVersionString( vers, 1 );
	
	if ( vers[ 0 ] <= 255 - STRLEN( "Version " ) )
	{
		memmove( vers + 1 + STRLEN( "Version " ), vers + 1, vers[ 0 ] );
		mempcpy( vers + 1, STR_LEN( "Version " ) );
		
		vers[ 0 ] += STRLEN( "Version " );
	}
	
	return true;
}

void draw_About_box()
{
	short v = kAboutBoxIconTopMargin;
	short h = kAboutBoxTextHorizontalMargin;
	
	Rect icon_bounds =
	{
		v,
		h,
		v + kAboutBoxIconHeight,
		h + kAboutBoxIconWidth,
	};
	
	mac::qd::plot_icon_id( icon_bounds, 128 );
	
	v = kAboutBoxTopMargin;
	
	h += kAboutBoxIconWidth + kAboutBoxIconToTextGap;
	
	Rect text_bounds =
	{
		v,
		h,
		v + kAboutBoxAppNameHeight,
		kAboutBoxWidth - kAboutBoxTextHorizontalMargin,
	};
	
	TextFont( 0 );  // Use system font
	TextSize( 0 );
	
	if ( TARGET_API_MAC_CARBON  &&  mac::sys::gestalt( 'sysv' ) >= 0x1000 )
	{
		TextSize( 14 );
		TextFace( bold );
	}
	
	TETextBox( name + 1, name[ 0 ], &text_bounds, teJustLeft );
	
	v += kAboutBoxAppNameHeight + kAboutBoxInterTextGap;
	
	text_bounds.top    = v;
	text_bounds.bottom = v + kAboutBoxDetailHeight;
	
	if ( TARGET_API_MAC_CARBON )
	{
		TextFace( 0 );
	}
	
	TextFont( 1 );  // Use application font, which should be Geneva
	TextSize( 9 );
	
	TETextBox( vers + 1, vers[ 0 ], &text_bounds, teJustLeft );
	
	short dv = kAboutBoxDetailHeight + kAboutBoxInterTextGap;
	
	text_bounds.top    += dv;
	text_bounds.bottom += dv;
	
	TETextBox( copy + 1, copy[ 0 ], &text_bounds, teJustLeft );
	
	text_bounds.left  = kAboutBoxTextHorizontalMargin;
	text_bounds.right = kAboutBoxWidth - kAboutBoxTextHorizontalMargin;
	
	dv = kAboutBoxDetailHeight + kAboutBoxInterSectionGap;
	
	text_bounds.top    += dv;
	text_bounds.bottom += dv;
	
	TETextBox( more + 1, more[ 0 ], &text_bounds, teJustCenter );
}

static inline
short get_menubar_height()
{
	return GetMBarHeight();
}

static inline
const Rect& window_structure_bounds( WindowRef window )
{
	/*
		This is not generic -- we assume the window is at v=0.
		
		Also, in the non-opaque structs case, we return an address
		which lies within a relocatable block -- handle with care. ;-)
	*/
	
#if ! OPAQUE_TOOLBOX_STRUCTS
	
	WindowPeek w = (WindowPeek) window;
	
	const Rect& box = w->strucRgn[0]->rgnBBox;
	
#else
	
	static Rect box;
	GetWindowBounds( window, kWindowStructureRgn, &box );
	
#endif
	
	return box;
}

#define WindowList (*(WindowRef*) 0x09D6)

static inline
WindowRef get_WindowList()
{
	return ACCESSOR_CALLS_ARE_FUNCTIONS ? GetWindowList() : WindowList;
}

static
WindowRef find_About_box()
{
	WindowRef w = get_WindowList();
	
	for ( ;  w != NULL;  w = GetNextWindow( w ) )
	{
		if ( GetWindowKind( w ) == kAboutWindowKind )
		{
			return w;
		}
	}
	
	return w;  // NULL
}

void show_About_box()
{
	if ( WindowRef aboutBox = find_About_box() )
	{
		SelectWindow( aboutBox );
		return;
	}
	
	using mac::app::new_window;
	
	static bool got_data = get_data();
	
	const Rect& screen_bounds = mac::qd::screenBits().bounds;
	
	short v_lower_bound = get_menubar_height();
	short v_upper_bound = screen_bounds.bottom;
	
	if ( WindowRef front = FrontWindow() )
	{
		v_upper_bound = window_structure_bounds( front ).top;
	}
	
	/*
		Create the window at v=0 and offscreen, so we can measure
		the structure and calculate a suitable window location.
	*/
	
	short top  = 0;
	short left = -16384;
	
	Rect rect = { top, left, top + kAboutBoxHeight, left + kAboutBoxWidth };
	
	Boolean vis      = true;
	Boolean closable = true;
	
	short proc = noGrowDocProc;
	
	WindowRef behind = (WindowRef) -1;
	
	WindowRef window = new_window( rect, title, vis, proc, behind, closable );
	
	SetWindowKind( window, kAboutWindowKind );
	
	/*
		With non-opaque Toolbox structs, the value assigned to `box`
		becomes invalid with the call to MoveWindow() below.
	*/
	
	const Rect& box = window_structure_bounds( window );
	
	short title_bar_height = -box.top;
	short structure_height = box.bottom - box.top;
	
	v_lower_bound += 2;
	v_upper_bound -= 2;
	
	short v_space = v_upper_bound - v_lower_bound;
	
	if ( v_space > structure_height )
	{
		v_lower_bound += (v_space - structure_height) / 2u;
	}
	
	v_lower_bound += title_bar_height;
	
	left = (screen_bounds.right - kAboutBoxWidth) / 2u;
	
	MoveWindow( window, left, v_lower_bound, false );
}

bool close_About_box( WindowRef window )
{
	const bool is_About = GetWindowKind( window ) == kAboutWindowKind;
	
	if ( is_About )
	{
		DisposeWindow( window );
	}
	
	return is_About;
}
