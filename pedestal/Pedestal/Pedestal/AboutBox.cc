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
#ifndef __TEXTEDIT__
#include <TextEdit.h>
#endif

// Standard C++
#include <memory>

// missing-macos
#ifdef MAC_OS_X_VERSION_10_7
#ifndef MISSING_QUICKDRAWTEXT_H
#include "missing/QuickdrawText.h"
#endif
#endif

// mac-qd-utils
#include "mac_qd/plot_icon_id.hh"

// MacFeatures
#include "MacFeatures/ColorQuickdraw.hh"

// Nitrogen
#include "Nitrogen/Icons.hh"
#include "Nitrogen/Quickdraw.hh"

// Pedestal
#include "Pedestal/BundleIcon.hh"
#include "Pedestal/BundleStrings.hh"
#include "Pedestal/CenteredText.hh"
#include "Pedestal/HIViewPlotIconRef.hh"
#include "Pedestal/OwnerResource.hh"
#include "Pedestal/View.hh"
#include "Pedestal/Window.hh"
#include "Pedestal/WindowStorage.hh"


#define STR_LEN( s )  "" s, (sizeof s - 1)


namespace Pedestal
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	
	
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
		
		const short topMargin = ::GetMBarHeight() + 18;  // FIXME:  Calculate title bar height
		
		BitMap screenBits = N::GetQDGlobalsScreenBits();
		
		short spareWidth = screenBits.bounds.right - bounds.right;
		
		short spareHeight = screenBits.bounds.bottom - bounds.bottom - topMargin;
		
		::OffsetRect( &bounds,
		              spareWidth / 2,
		              topMargin + spareHeight / 3 );
	}
	
	class AboutBoxView : public View
	{
		public:
			void Draw( const Rect& bounds, bool erasing );
			
			void DrawInContext( CGContextRef context, CGRect bounds );
	};
	
	void AboutBoxView::Draw( const Rect& bounds, bool erasing )
	{
		if ( erasing )
		{
			N::EraseRect( bounds );
		}
		
		short top  = bounds.top  + kAboutBoxTopMargin;
		short left = bounds.left + kAboutBoxIconHorizontalMargin;
		
		const Rect iconBounds =
		{
			top,
			left,
			top  + kAboutBoxIconHeight,
			left + kAboutBoxIconWidth,
		};
		
		mac::qd::plot_icon_id( iconBounds, 128 );
		
		top += kAboutBoxIconEdgeLength + kAboutBoxIconToTextGap;
		left = bounds.left + kAboutBoxTextHorizontalMargin;
		
		const Rect nameBounds =
		{
			top,
			left,
			top  + kAboutBoxAppNameHeight,
			left + kAboutBoxTextWidth,
		};
		
		TextFont( 0 );  // Use system font
		TextSize( 0 );
		
		const OSType creator = GetCreatorFromBNDL();
		
		Str255 name = "\p" "Pedestal";
		Str255 data = "\p";
		
		GetOwnerResourceName( creator, name );
		GetOwnerResourceData( creator, data );
		
		TETextBox( name + 1, name[ 0 ], &nameBounds, teJustCenter );
		
		top += kAboutBoxAppNameHeight + kAboutBoxInterTextGap;
		
		Rect detailBounds =
		{
			top,
			left,
			top  + kAboutBoxDetailHeight,
			left + kAboutBoxTextWidth,
		};
		
		TextFont( 1 );  // Use application font, which should be Geneva
		TextSize( 9 );
		
		TETextBox( STR_LEN( VERSION_FALLBACK ), &detailBounds, teJustCenter );
		
		detailBounds.top    += kAboutBoxDetailHeight + kAboutBoxInterTextGap;
		detailBounds.bottom += kAboutBoxDetailHeight + kAboutBoxInterTextGap;
		
		TETextBox( data + 1, data[ 0 ], &detailBounds, teJustCenter );
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
	
	void AboutBoxView::DrawInContext( CGContextRef context, CGRect bounds )
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
		
		HIViewPlotIconRef( context, iconBounds, BundleIcon() );
		
		x = bounds.origin.x + kAboutBoxTextHorizontalMargin;
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
			x,
			y,
			kAboutBoxTextWidth,
			kAboutBoxDetailHeight,
		};
		
		DrawAboutBoxDetail( GetBundleVersion(), detailBounds, context );
		
		detailBounds.origin.y += kAboutBoxDetailHeight + kAboutBoxInterTextGap;
		
		DrawAboutBoxDetail( GetBundleGetInfoString(), detailBounds, context );
	}
	
	
	static std::auto_ptr< Window > sAboutBox;
	
	static
	void AboutClosed( WindowRef window )
	{
		sAboutBox.reset();
	}
	
	static std::auto_ptr< Window > NewAboutBox()
	{
		const Mac::WindowAttributes attrs = Mac::kWindowCloseBoxAttribute
		                                #ifdef MAC_OS_X_VERSION_10_2
		                                  | Mac::kWindowCompositingAttribute
		                                #endif
		                                #ifdef MAC_OS_X_VERSION_10_7
		                                  | Mac::kWindowHighResolutionCapableAttribute
		                                #endif
		                                  ;
		
		Rect bounds = { 0, 0, kAboutBoxHeight, kAboutBoxWidth };
		
		CenterWindowRect( bounds );
		
	#if ! TARGET_API_MAC_CARBON
		
		n::owned< WindowRef > window = CreateWindow( bounds,
		                                             "\p" "About",
		                                             true,
		                                             Mac::noGrowDocProc );
		
	#else
		
		n::owned< WindowRef > window = CreateWindow( Mac::kDocumentWindowClass,
		                                             attrs,
		                                             bounds );
		
	#endif
		
		if ( MacFeatures::Has_ColorQuickdraw() )
		{
			N::RGBBackColor( kAboutBoxBackgroundColor );
		}
		
		std::auto_ptr< Window > owner( new Window( window ) );
		
		set_window_closed_proc( owner->Get(), &AboutClosed );
		
		boost::intrusive_ptr< View > view( new AboutBoxView() );
		
		set_window_view( owner->Get(), view.get() );
		
	#if TARGET_API_MAC_CARBON
		
		ShowWindow( owner->Get() );
		
	#endif
		
		return owner;
	}
	
	
	void ShowAboutBox()
	{
		if ( sAboutBox.get() == NULL )
		{
			sAboutBox = NewAboutBox();
		}
		else
		{
			SelectWindow( sAboutBox->Get() );
		}
	}
	
}
