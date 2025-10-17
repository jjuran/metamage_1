/*
	CGAboutBox.cc
	-------------
*/

#include "mac_app/CGAboutBox.hh"

#if TARGET_RT_MAC_MACHO

// missing-macos
#if ! __LP64__
#ifdef MAC_OS_X_VERSION_10_7
#ifndef MISSING_QUICKDRAWTEXT_H
#include "missing/QuickdrawText.h"
#endif
#endif
#endif

// mac-config
#include "mac_config/compositing-mode.hh"
#include "mac_config/upp-macros.hh"

// mac-qd-utils
#include "mac_qd/CGContext_for_port.hh"
#include "mac_qd/get_portRect.hh"
#include "mac_qd/globals/screenBits.hh"

// mac-icon-utils
#include "mac_icon/plot_IconRef.hh"

// mac-ui-utils
#include "mac_ui/TETextBox_CFString.hh"
#include "mac_ui/windows.hh"

// mac-cg-utils
#include "mac_cg/self_masked_drawing.hh"

// mac-app-utils
#include "mac_app/about_box_metrics.hh"
#include "mac_app/bundle_strings.hh"
#include "mac_app/copy_bundle_icon.hh"
#include "mac_app/draw_centered_text.hh"


/*
	We should only use Core Graphics if small fonts are readable.
	Mac OS X 10.4 has subpixel font rendering enabled by default.
*/

#ifndef CONFIG_HITHEME_LABELS
#ifdef MAC_OS_X_VERSION_10_4
#define CONFIG_HITHEME_LABELS  1
#else
#define CONFIG_HITHEME_LABELS  0
#endif
#endif

#define LENGTH( array )  (sizeof (array) / sizeof (array)[0])

#define APP_NAME_FONT  "Lucida Grande Bold"
#define APP_NAME_SIZE  14


#pragma exceptions off


namespace mac {
namespace app {

static
void DrawApplicationName( CGContextRef context )
{
	/*
		Nudge the application name down one pixel
		to match the behavior of Cocoa About boxes.
	*/
	
	const short kAboutBoxAppNameVerticalNudge = 1;
	
	short x = kAboutBoxTextHorizontalMargin;
	short y = kAboutBoxTopMargin
	        + kAboutBoxIconHeight
	        + kAboutBoxIconToTextGap
	        + kAboutBoxAppNameVerticalNudge;
	
#ifdef MAC_OS_X_VERSION_10_5
	
	CTFontRef font = CTFontCreateWithName( CFSTR( APP_NAME_FONT ),
	                                       APP_NAME_SIZE,
	                                       NULL );
	
	CFStringRef keys[] = { kCTFontAttributeName };
	CFTypeRef values[] = { font };
	
	CFDictionaryRef attributes;
	attributes = CFDictionaryCreate( kCFAllocatorDefault,
	                                 (const void**) keys,
	                                 (const void**) values,
	                                 LENGTH( keys ),
	                                 &kCFTypeDictionaryKeyCallBacks,
	                                 &kCFTypeDictionaryValueCallBacks );
	
	CFAttributedStringRef attrString;
	attrString = CFAttributedStringCreate( kCFAllocatorDefault,
	                                       GetBundleName(),
	                                       attributes );
	
	CTLineRef line = CTLineCreateWithAttributedString( attrString );
	
	CGFloat ascent;
	CGFloat descent;
	CGFloat leading;
	
	double lineWidth = CTLineGetTypographicBounds( line, &ascent, &descent, &leading );
	
	double pen_x = CTLineGetPenOffsetForFlush( line, 0.5, kAboutBoxWidth );
	double pen_y = y + leading + ascent;
	
	CGContextSaveGState( context );
	
	CGContextScaleCTM( context, 1.0, -1.0 );
	
	CGContextSetTextPosition( context, pen_x, -pen_y );
	
	CTLineDraw( line, context );
	
	CGContextRestoreGState( context );
	
	CFRelease( attrString );
	CFRelease( attributes );
	CFRelease( line );
	CFRelease( font );
	
#else
	
	const Rect bounds =
	{
		y,
		x,
		y + kAboutBoxAppNameHeight,
		x + kAboutBoxTextWidth,
	};
	
	draw_centered_text( GetBundleName(), bounds, APP_NAME_FONT, APP_NAME_SIZE );
	
#endif
}

#if ! __LP64__

static
void DrawLabel_TE( CGContextRef context, const CGRect& rect, CFStringRef text )
{
	mac::cg::self_masked_drawing drawing( context, rect );
	
	if ( drawing.works() )
	{
		TextFont( 1 );  // Use application font, which should be Geneva
		TextSize( 9 );
		
		mac::ui::TETextBox_CFString( text, drawing.rect(), teJustCenter );
	}
}

#endif

static
void DrawAboutBoxDetail( CFStringRef    text,
                         const CGRect&  bounds,
                         CGContextRef   context )
{
	/*
		We can't use MLTE/TXN + ATSU here, because "Lucida Grande Regular"
		doesn't exist, and "Lucida Grande" is too bold.  Also, there's a
		perfectly suitable theme font for the purpose.
	*/
	
#if CONFIG_HITHEME_LABELS
	
	OSStatus err;
	
	HIThemeTextInfo textInfo =
	{
		kHIThemeTextInfoVersionZero,
		0,
		kThemeLabelFont,  // Lucida Grande Regular 10
		kHIThemeTextHorizontalFlushCenter,
	};
	
	err = HIThemeDrawTextBox( text,
	                          &bounds,
	                          &textInfo,
	                          context,
	                          kHIThemeOrientationNormal );
	
#else
	
	DrawLabel_TE( context, bounds, text );
	
#endif
}

static
CFStringRef CopyVersionString()
{
	CFStringRef shortVersion = GetBundleShortVersionString();
	CFStringRef buildNumber  = GetBundleVersion();
	
	if ( shortVersion == NULL  &&  buildNumber == NULL )
	{
		return NULL;
	}
	
	CFMutableStringRef versionString = CFStringCreateMutable( NULL, 0 );
	
	const CFStringEncoding macRoman = kCFStringEncodingMacRoman;
	
	CFStringAppendPascalString( versionString, "\p" "Version ", macRoman );
	
	if ( shortVersion == NULL )
	{
		CFStringAppend( versionString, buildNumber );
		
		return versionString;
	}
	
	CFStringAppend( versionString, shortVersion );
	
	if ( buildNumber != NULL )
	{
		CFStringAppendPascalString( versionString, "\p" " (", macRoman );
		
		CFStringAppend( versionString, buildNumber );
		
		CFStringAppendPascalString( versionString, "\p" ")", macRoman );
	}
	
	return versionString;
}

static
void draw_AboutBox( CGContextRef context, CGRect bounds )
{
	CGContextSetGrayFillColor( context, 1.0 * 0xEEEE / 0xFFFF, 1.0 );
	CGContextFillRect( context, bounds );
	
	float x = bounds.origin.x + kAboutBoxIconHorizontalMargin;
	float y = bounds.origin.y + kAboutBoxTopMargin;
	
	const CGRect iconBounds =
	{
		{ x, y },
		{ kAboutBoxIconWidth, kAboutBoxIconHeight },
	};
	
	if ( IconRef icon = copy_bundle_icon() )
	{
		mac::icon::plot_IconRef( context, iconBounds, icon );
		
		ReleaseIconRef( icon );
	}
	
	x = bounds.origin.x + kAboutBoxTextHorizontalMargin;
	y += kAboutBoxIconHeight + kAboutBoxIconToTextGap;
	
	CGContextSetGrayFillColor( context, 0.0, 1.0 );  // black
	
	DrawApplicationName( context );
	
	y += kAboutBoxAppNameHeight + kAboutBoxInterTextGap;
	
	CGRect detailBounds =
	{
		{ x, y, },
		{
			kAboutBoxTextWidth,
			kAboutBoxDetailHeight,
		},
	};
	
	if ( CFStringRef version = CopyVersionString() )
	{
		DrawAboutBoxDetail( version, detailBounds, context );
		
		CFRelease( version );
	}
	
	detailBounds.origin.y += kAboutBoxDetailHeight + kAboutBoxInterTextGap;
	
	DrawAboutBoxDetail( GetBundleGetInfoString(), detailBounds, context );
}

#if ! __LP64__

static
pascal OSStatus window_DrawContent( EventHandlerCallRef  handler,
                                    EventRef             event,
                                    void*                userData )
{
	OSStatus err;
	
	WindowRef window;
	CGContextRef context = NULL;
	
	err = GetEventParameter( event,
	                         kEventParamDirectObject,
	                         typeWindowRef,
	                         NULL,
	                         sizeof window,
	                         NULL,
	                         &window );
	
	if ( err == noErr )
	{
		using mac::qd::CGContext_for_port;
		
		CGrafPtr port = GetWindowPort( window );
		
		CGContext_for_port context( port );
		
		Rect portRect = mac::qd::get_portRect( port );
		
		int width  = portRect.right - portRect.left;
		int height = portRect.bottom - portRect.top;
		
		CGRect bounds;
		
		bounds.origin.x = portRect.left;
		bounds.origin.y = portRect.top;
		bounds.size.width  = width;
		bounds.size.height = height;
		
		draw_AboutBox( context, bounds );
	}
	
	return err;
}

DEFINE_CARBON_UPP( EventHandler, window_DrawContent )

static EventTypeSpec windowDrawContent_event[] =
{
	{ kEventClassWindow, kEventWindowDrawContent },
};

static
pascal OSStatus window_ControlDraw( EventHandlerCallRef  handler,
                                    EventRef             event,
                                    void*                userData )
{
	OSStatus err;
	
	ControlRef control;
	CGContextRef context = NULL;
	
	(err = GetEventParameter( event,
	                          kEventParamDirectObject,
	                          typeControlRef,
	                          NULL,
	                          sizeof control,
	                          NULL,
	                          &control ))  ||
	(err = GetEventParameter( event,
	                          kEventParamCGContextRef,
	                          typeCGContextRef,
	                          NULL,
	                          sizeof context,
	                          NULL,
	                          &context ));
	
#ifdef MAC_OS_X_VERSION_10_2
	
	if ( err == noErr )
	{
		HIRect bounds;
		HIViewGetBounds( control, &bounds );
		
		draw_AboutBox( context, bounds );
	}
	
#endif
	
	return err;
}

DEFINE_CARBON_UPP( EventHandler, window_ControlDraw )

static EventTypeSpec controlDraw_event[] =
{
	{ kEventClassControl, kEventControlDraw },
};

static
void CenterWindowRect( Rect& bounds )
{
	// Pre-conditions:  bounds is set to { 0, 0, v, h }
	
	const Rect& screenBounds = mac::qd::screenBits().bounds;
	
	short spareWidth  = screenBounds.right  - bounds.right;
	short spareHeight = screenBounds.bottom - bounds.bottom;
	
	/*
		Use unsigned division for smaller 68K code.
		This assumes that main screen coordinates are non-negative,
		and that the window fits within the screen bounds.
	*/
	
	bounds.left  += spareWidth / 2u;
	bounds.right += spareWidth / 2u;
	
	bounds.top    += spareHeight * 3 / 8u;
	bounds.bottom += spareHeight * 3 / 8u;
}

WindowRef CGAboutBox_make()
{
	const WindowAttributes attrs = kWindowCloseBoxAttribute
	                             | kWindowStandardHandlerAttribute
	                           #if CONFIG_COMPOSITING
	                             | kWindowCompositingAttribute
	                           #endif
	                           #ifdef MAC_OS_X_VERSION_10_3
	                             | kWindowAsyncDragAttribute
	                           #endif
	                           #ifdef MAC_OS_X_VERSION_10_7
	                             | kWindowHighResolutionCapableAttribute
	                           #endif
	                             ;
	
	Rect bounds = { 0, 0, kAboutBoxHeight, kAboutBoxWidth };
	
	CenterWindowRect( bounds );
	
	OSStatus err;
	WindowRef window;
	err = CreateNewWindow( kDocumentWindowClass, attrs, &bounds, &window );
	
	if ( err != noErr )
	{
		return NULL;
	}
	
	if ( CONFIG_COMPOSITING )
	{
	#ifdef MAC_OS_X_VERSION_10_2
		
		OSStatus err;
		HIViewRef root = HIViewGetRoot( window );
		
		HIViewRef content;
		err = HIViewFindByID( root, kHIViewWindowContentID, &content );
		
		err = InstallControlEventHandler( content,
		                                  UPP_ARG( window_ControlDraw ),
		                                  1,
		                                  controlDraw_event,
		                                  NULL,
		                                  NULL );
		
		mac::ui::invalidate_window( window );
		
	#endif
	}
	else
	{
		err = InstallWindowEventHandler( window,
		                                 UPP_ARG( window_DrawContent ),
		                                 1,
		                                 windowDrawContent_event,
		                                 NULL,
		                                 NULL );
	}
	
	ShowWindow( window );
	
	return window;
}

#else  // #if ! __LP64__

int dummy;

#endif

}
}

#endif  // #if TARGET_RT_MAC_MACHO
