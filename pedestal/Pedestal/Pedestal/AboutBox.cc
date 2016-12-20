/*	===========
 *	AboutBox.cc
 *	===========
 */

#include "Pedestal/AboutBox.hh"

// Standard C++
#include <memory>

// Nitrogen
#include "Nitrogen/Icons.hh"
#include "Nitrogen/Quickdraw.hh"

// Pedestal
#include "Pedestal/BundleIcon.hh"
#include "Pedestal/HIViewPlotIconRef.hh"
#include "Pedestal/View.hh"
#include "Pedestal/Window.hh"
#include "Pedestal/WindowStorage.hh"


namespace Pedestal
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	
	
	const int kAboutBoxIconEdgeLength       =  64;
	
	const int kAboutBoxIconWidth            = kAboutBoxIconEdgeLength;
	const int kAboutBoxIconHorizontalMargin = 110;
	
	const int kAboutBoxTopMargin            =   8;
	const int kAboutBoxIconHeight           = kAboutBoxIconEdgeLength;
	const int kAboutBoxBottomMargin         =  20;
	
	const int kAboutBoxWidth = kAboutBoxIconWidth
	                         + kAboutBoxIconHorizontalMargin * 2;
	
	const int kAboutBoxHeight = kAboutBoxTopMargin
	                          + kAboutBoxIconHeight
	                          + kAboutBoxBottomMargin;
	
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
		
		const short top  = bounds.top  + kAboutBoxTopMargin;
		const short left = bounds.left + kAboutBoxIconHorizontalMargin;
		
		const Rect iconBounds =
		{
			top,
			left,
			top  + kAboutBoxIconHeight,
			left + kAboutBoxIconWidth,
		};
		
		N::PlotIconID( iconBounds,
		               N::IconAlignmentType(),
		               N::IconTransformType(),
		               N::ResID( 128 ) );
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
		
		SetWTitle( window, "\p" "About" );
		
	#endif
		
		N::RGBBackColor( kAboutBoxBackgroundColor );
		
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
