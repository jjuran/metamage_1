/*	===========
 *	AboutBox.cc
 *	===========
 */

#include "Pedestal/AboutBox.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif
#ifndef __TEXTEDIT__
#include <TextEdit.h>
#endif

// missing-macos
#ifdef MAC_OS_X_VERSION_10_7
#ifndef MISSING_QUICKDRAW_H
#include "missing/Quickdraw.h"
#endif
#ifndef MISSING_QUICKDRAWTEXT_H
#include "missing/QuickdrawText.h"
#endif
#endif

// Standard C
#include <string.h>

// more-libc
#include "more/string.h"

// mac-config
#include "mac_config/color-quickdraw.hh"

// mac-sys-utils
#include "mac_sys/has/ColorQuickDraw.hh"

// mac-rsrc-utils
#include "mac_rsrc/get_string_from_resource.hh"

// mac-qd-utils
#include "mac_qd/plot_icon_id.hh"
#include "mac_qd/globals/screenBits.hh"

// mac-icon-utils
#include "mac_icon/plot_IconRef.hh"

// mac-sys-utils
#include "mac_sys/gestalt.hh"

// debug
#include "debug/boost_assert.hh"

// Boost
#include <boost/intrusive_ptr.hpp>

// Nitrogen
#include "Carbon/CF/Types/CFMutableStringRef.hh"

// Pedestal
#include "Pedestal/BundleIcon.hh"
#include "Pedestal/BundleStrings.hh"
#include "Pedestal/CenteredText.hh"
#include "Pedestal/OwnerResource.hh"
#include "Pedestal/View.hh"
#include "Pedestal/WindowStorage.hh"


/*
	Mac OS X 10.2 supports compositing mode, but it has some issues,
	which we'll avoid by not using it.  In particular, our Quartz-only
	code path relies on HIThemeDrawTextBox(), which isn't available
	until 10.3.
*/

#ifndef CONFIG_COMPOSITING
#ifdef MAC_OS_X_VERSION_10_3  // not 10.2
#define CONFIG_COMPOSITING  1
#else
#define CONFIG_COMPOSITING  0
#endif
#endif

#define STRLEN( s )  (sizeof "" s - 1)

#define STR_LEN( s )  "" s, (sizeof s - 1)


static inline
bool has_color_quickdraw()
{
	return CONFIG_COLOR_QUICKDRAW_GRANTED  ||  mac::sys::has_ColorQuickDraw();
}

namespace Pedestal
{
	
	namespace n = nucleus;
	
	
	const int kAboutBoxIconEdgeLength       =  32 << TARGET_API_MAC_CARBON;
	
	const int kAboutBoxIconWidth            = kAboutBoxIconEdgeLength;
	const int kAboutBoxIconHorizontalMargin = 110;
	const int kAboutBoxTextHorizontalMargin =  24;
	
	const int kAboutBoxTopMargin            =   8;
	const int kAboutBoxIconHeight           = kAboutBoxIconEdgeLength;
	const int kAboutBoxIconToTextGap        =  12;
	const int kAboutBoxAppNameHeight        =  19;
	const int kAboutBoxInterTextGap         =   8;
	const int kAboutBoxDetailHeight         =  12;
	const int kAboutBoxBottomMargin         =  20;
	
	const int kAboutBoxWidth = kAboutBoxIconWidth
	                         + kAboutBoxIconHorizontalMargin * 2;
	
	const int kAboutBoxHeight = kAboutBoxTopMargin
	                          + kAboutBoxIconHeight
	                          + kAboutBoxIconToTextGap
	                          + kAboutBoxAppNameHeight
	                          + kAboutBoxInterTextGap
	                          + kAboutBoxDetailHeight
	                          + kAboutBoxInterTextGap
	                          + kAboutBoxDetailHeight
	                          + kAboutBoxBottomMargin;
	
	const int kAboutBoxTextWidth = kAboutBoxWidth
	                             - kAboutBoxTextHorizontalMargin * 2;
	
	static const RGBColor kAboutBoxBackgroundColor = { 0xEEEE, 0xEEEE, 0xEEEE };
	
	
	static void CenterWindowRect( Rect& bounds )
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
		
		::OffsetRect( &bounds,
		              spareWidth / 2u,
		              spareHeight * 3 / 8u );
	}
	
	class AboutBoxView : public View
	{
		public:
			void DrawInContext( CGContextRef context, CGRect bounds );
	};
	
	static
	void draw_About_box( const Rect& bounds )
	{
		EraseRect( &bounds );
		
		short top  = kAboutBoxTopMargin;
		short left = kAboutBoxIconHorizontalMargin;
		
		const Rect iconBounds =
		{
			top,
			left,
			top  + kAboutBoxIconHeight,
			left + kAboutBoxIconWidth,
		};
		
		top += kAboutBoxIconEdgeLength + kAboutBoxIconToTextGap;
		left = kAboutBoxTextHorizontalMargin;
		
		const Rect nameBounds =
		{
			top,
			left,
			top  + kAboutBoxAppNameHeight,
			left + kAboutBoxTextWidth,
		};
		
		TextFont( 0 );  // Use system font
		TextSize( 0 );
		
		if ( TARGET_API_MAC_CARBON  &&  mac::sys::gestalt( 'sysv' ) >= 0x1000 )
		{
			TextSize( 14 );
			TextFace( bold );
		}
		
		const OSType creator = GetCreatorFromBNDL();
		
		Str255 name = "\p" "Pedestal";
		Str255 data = "\p";
		Str255 vers = "\p" VERSION_FALLBACK;
		
		if ( Handle h = GetResource( creator, 0 ) )
		{
			GetResInfo( h, NULL, NULL, name );
			
			mac::rsrc::get_string_from_handle( data, h );
		}
		
		if ( mac::rsrc::get_vers_ShortVersionString( vers, 1 ) )
		{
			if ( vers[ 0 ] <= 255 - STRLEN( "Version " ) )
			{
				memmove( vers + 1 + STRLEN( "Version " ), vers + 1, vers[ 0 ] );
				mempcpy( vers + 1, STR_LEN( "Version " ) );
				
				vers[ 0 ] += STRLEN( "Version " );
			}
		}
		
		TETextBox( name + 1, name[ 0 ], &nameBounds, teJustCenter );
		
		top += kAboutBoxAppNameHeight + kAboutBoxInterTextGap;
		
		Rect detailBounds =
		{
			top,
			left,
			top  + kAboutBoxDetailHeight,
			left + kAboutBoxTextWidth,
		};
		
		if ( TARGET_API_MAC_CARBON )
		{
			TextFace( 0 );
		}
		
		TextFont( 1 );  // Use application font, which should be Geneva
		TextSize( 9 );
		
		TETextBox( vers + 1, vers[ 0 ], &detailBounds, teJustCenter );
		
		detailBounds.top    += kAboutBoxDetailHeight + kAboutBoxInterTextGap;
		detailBounds.bottom += kAboutBoxDetailHeight + kAboutBoxInterTextGap;
		
		TETextBox( data + 1, data[ 0 ], &detailBounds, teJustCenter );
		
		BackColor( whiteColor );
		
		mac::qd::plot_icon_id( iconBounds, 128 );
	}
	
	static inline
	void DrawApplicationName( CFStringRef text, const Rect& bounds )
	{
		DrawCenteredText( text, bounds, "Lucida Grande Bold", 14 );
	}
	
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
		
	#ifdef MAC_OS_X_VERSION_10_3
		
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
		
	#endif
	}
	
	static
	n::owned< CFStringRef > VersionString()
	{
		CFStringRef shortVersion = GetBundleShortVersionString();
		CFStringRef buildNumber  = GetBundleVersion();
		
		if ( shortVersion == NULL  &&  buildNumber == NULL )
		{
			return n::owned< CFStringRef >::seize( CFSTR( VERSION_FALLBACK ) );
		}
		
		CFMutableStringRef versionString = CFStringCreateMutable( NULL, 0 );
		
		const CFStringEncoding macRoman = kCFStringEncodingMacRoman;
		
		CFStringAppendPascalString( versionString, "\p" "Version ", macRoman );
		
		if ( shortVersion == NULL )
		{
			CFStringAppend( versionString, buildNumber );
			
			return n::owned< CFStringRef >::seize( versionString );
		}
		
		CFStringAppend( versionString, shortVersion );
		
		if ( buildNumber != NULL )
		{
			CFStringAppendPascalString( versionString, "\p" " (", macRoman );
			
			CFStringAppend( versionString, buildNumber );
			
			CFStringAppendPascalString( versionString, "\p" ")", macRoman );
		}
		
		return n::owned< CFStringRef >::seize( versionString );
	}
	
	void AboutBoxView::DrawInContext( CGContextRef context, CGRect bounds )
	{
		CGContextSetGrayFillColor( context, 1.0 * 0xEEEE / 0xFFFF, 1.0 );
		CGContextFillRect( context, bounds );
		
		float x = kAboutBoxIconHorizontalMargin;
		float y = kAboutBoxTopMargin;
		
		const CGRect iconBounds =
		{
			{ x, y },
			{ kAboutBoxIconWidth, kAboutBoxIconHeight },
		};
		
		mac::icon::plot_IconRef( context, iconBounds, BundleIcon() );
		
		x = kAboutBoxTextHorizontalMargin;
		y += kAboutBoxIconHeight + kAboutBoxIconToTextGap;
		
		Rect nameBounds =
		{
			(short) y,
			(short) x,
			(short) y + kAboutBoxAppNameHeight,
			(short) x + kAboutBoxTextWidth,
		};
		
		CGContextSetGrayFillColor( context, 0.0, 1.0 );  // black
		
		DrawApplicationName( GetBundleName(), nameBounds );
		
		y += kAboutBoxAppNameHeight + kAboutBoxInterTextGap;
		
		CGRect detailBounds =
		{
			{ x, y, },
			{
				kAboutBoxTextWidth,
				kAboutBoxDetailHeight,
			},
		};
		
		DrawAboutBoxDetail( VersionString(), detailBounds, context );
		
		detailBounds.origin.y += kAboutBoxDetailHeight + kAboutBoxInterTextGap;
		
		DrawAboutBoxDetail( GetBundleGetInfoString(), detailBounds, context );
	}
	
	
	static n::owned< WindowRef > sAboutBox;
	
#if CONFIG_COMPOSITING
#define GetWindowPic( window )  NULL
#define KillPicture( picture )  /**/
#endif
	
	static
	void AboutClosed( WindowRef window )
	{
		PicHandle pic = GetWindowPic( window );
		
		sAboutBox.reset();
		
		KillPicture( pic );
	}
	
#undef GetWindowPic
#undef KillPicture
	
	static
	n::owned< WindowRef > NewAboutBox()
	{
		const WindowAttributes attrs = kWindowCloseBoxAttribute
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
		
	#if ! TARGET_API_MAC_CARBON
		
		n::owned< WindowRef > window = CreateWindow( bounds,
		                                             "\p" "About",
		                                             true,
		                                             noGrowDocProc );
		
	#else
		
		n::owned< WindowRef > window = CreateWindow( kDocumentWindowClass,
		                                             attrs,
		                                             bounds );
		
	#endif
		
		if ( CONFIG_COLOR_QUICKDRAW  &&  has_color_quickdraw() )
		{
			RGBBackColor( &kAboutBoxBackgroundColor );
		}
		
		set_window_closed_proc( window, &AboutClosed );
		
	#if CONFIG_COMPOSITING
		
		boost::intrusive_ptr< View > view( new AboutBoxView() );
		
		set_window_view( window, view.get() );
		
	#else
		
		Rect frame = { 0, 0, kAboutBoxHeight, kAboutBoxWidth };
		
		PicHandle pic = OpenPicture( &frame );
		
		draw_About_box( frame );
		
		ClosePicture();
		
		SetWindowPic( window, pic );
		
	#endif
		
	#if TARGET_API_MAC_CARBON
		
		ShowWindow( window );
		
	#endif
		
		return window;
	}
	
	
	void ShowAboutBox()
	{
		if ( sAboutBox.get() == NULL )
		{
			sAboutBox = NewAboutBox();
		}
		else
		{
			SelectWindow( sAboutBox );
		}
	}
	
}
