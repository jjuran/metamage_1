/*
	QDAboutBox.cc
	-------------
*/

#include "mac_app/QDAboutBox.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif
#ifndef __TEXTEDIT__
#include <TextEdit.h>
#endif

// missing-macos
#if ! __LP64__
#ifdef MAC_OS_X_VERSION_10_7
#ifndef MISSING_QUICKDRAW_H
#include "missing/Quickdraw.h"
#endif
#ifndef MISSING_QUICKDRAWTEXT_H
#include "missing/QuickdrawText.h"
#endif
#endif
#endif

// mac-config
#include "mac_config/color-quickdraw.hh"

// mac-sys-utils
#include "mac_sys/gestalt.hh"
#include "mac_sys/has/ColorQuickDraw.hh"

// mac-qd-utils
#include "mac_qd/plot_icon_id.hh"
#include "mac_qd/globals/screenBits.hh"

// mac-icon-utils
#include "mac_icon/plot_IconRef.hh"

// mac-app-utils
#include "mac_app/about_box_metrics.hh"
#include "mac_app/copy_bundle_icon.hh"
#include "mac_app/new_window.hh"


#pragma exceptions off


namespace mac {
namespace app {

ConstStr255Param gAboutBoxTitle = "\p" "About";
ConstStr255Param gAboutBoxAppName;
ConstStr255Param gAboutBoxAppVers;
ConstStr255Param gAboutBoxAppCopy;

#if ! __LP64__

static const RGBColor kRGBWhite                = { 0xFFFF, 0xFFFF, 0xFFFF };
static const RGBColor kAboutBoxBackgroundColor = { 0xEEEE, 0xEEEE, 0xEEEE };

static inline
bool has_color_quickdraw()
{
	return CONFIG_COLOR_QUICKDRAW_GRANTED  ||  mac::sys::has_ColorQuickDraw();
}


static
void centered_text_box( ConstStr255Param text, const Rect& bounds )
{
	TETextBox( text + 1, text[ 0 ], &bounds, teJustCenter );
}

static
void draw_About_box()
{
	short v = kAboutBoxTopMargin;
	short h = kAboutBoxIconHorizontalMargin;
	
	Rect icon_bounds =
	{
		v,
		h,
		v + kAboutBoxIconHeight,
		h + kAboutBoxIconWidth,
	};
	
	v += kAboutBoxIconEdgeLength + kAboutBoxIconToTextGap;
	h  = kAboutBoxTextHorizontalMargin;
	
	Rect text_bounds =
	{
		v,
		h,
		v + kAboutBoxAppNameHeight,
		h + kAboutBoxTextWidth,
	};
	
	TextFont( 0 );  // Use system font
	TextSize( 0 );
	
	if ( TARGET_API_MAC_CARBON  &&  mac::sys::gestalt( 'sysv' ) >= 0x1000 )
	{
		TextSize( 14 );
		TextFace( bold );
	}
	
	centered_text_box( gAboutBoxAppName, text_bounds );
	
	v += kAboutBoxAppNameHeight + kAboutBoxInterTextGap;
	
	text_bounds.top    = v;
	text_bounds.bottom = v + kAboutBoxDetailHeight;
	
	if ( TARGET_API_MAC_CARBON )
	{
		TextFace( 0 );
	}
	
	TextFont( 1 );  // Use application font, which should be Geneva
	TextSize( 9 );
	
	centered_text_box( gAboutBoxAppVers, text_bounds );
	
	short dv = kAboutBoxDetailHeight + kAboutBoxInterTextGap;
	
	text_bounds.top    += dv;
	text_bounds.bottom += dv;
	
	centered_text_box( gAboutBoxAppCopy, text_bounds );
	
	if ( TARGET_RT_MAC_MACHO )
	{
		if ( IconRef icon = copy_bundle_icon() )
		{
			mac::icon::plot_IconRef( icon_bounds, icon );
			
			ReleaseIconRef( icon );
			
			return;
		}
	}
	
	if ( CONFIG_COLOR_QUICKDRAW  &&  has_color_quickdraw() )
	{
		RGBBackColor( &kRGBWhite );
	}
	
	mac::qd::plot_icon_id( icon_bounds, 128 );
}

WindowRef QDAboutBox_make()
{
	ConstStr255Param title = gAboutBoxTitle;
	
	Boolean vis      = false;
	Boolean closable = true;
	
	short proc = noGrowDocProc;
	
	const Rect& screen_bounds = mac::qd::screenBits().bounds;
	
	short left = (screen_bounds.right - kAboutBoxWidth) / 2u;
	short top  = 40 + TARGET_API_MAC_CARBON * 8;
	
	top += (screen_bounds.bottom - top - kAboutBoxHeight) / 3u;
	
	Rect bounds = { top, left, top + kAboutBoxHeight, left + kAboutBoxWidth };
	
	WindowRef behind = (WindowRef) -1;
	WindowRef window;
	
	window = mac::app::new_window( bounds, title, vis, proc, behind, closable );
	
	SetPortWindowPort( window );
	
	Rect frame = { 0, 0, kAboutBoxHeight, kAboutBoxWidth };
	
	PicHandle pic = OpenPicture( &frame );
	
	if ( CONFIG_COLOR_QUICKDRAW  &&  has_color_quickdraw() )
	{
		RGBBackColor( &kAboutBoxBackgroundColor );
	}
	
	EraseRect( &frame );
	
	draw_About_box();
	
	ClosePicture();
	
	SetWindowPic( window, pic );
	
	ShowWindow( window );
	
	return window;
}

void QDAboutBox_close( WindowRef window )
{
	PicHandle pic = GetWindowPic( window );
	
	DisposeWindow( window );
	
	KillPicture( pic );
}

#endif  // #if ! __LP64__

}
}
