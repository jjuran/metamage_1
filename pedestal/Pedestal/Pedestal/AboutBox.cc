/*	===========
 *	AboutBox.cc
 *	===========
 */

#include "Pedestal/AboutBox.hh"

// MacFeatures
#include "MacFeatures/ColorQuickdraw.hh"

// Pedestal
#include "Pedestal/EmptyView.hh"
#include "Pedestal/GeneratedGraphic.hh"
#include "Pedestal/GraphicView.hh"
#include "Pedestal/UniqueWindowOwner.hh"
#include "Pedestal/UserWindow.hh"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	
	
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
	
	class AboutFunction
	{
		public:
			RGBColor operator()( double x,
			                     double y,
			                     double t ) const
			{
				return DenormalizeRGBColor( x, y, t );
			}
	};
	
	typedef GraphicView< GeneratedGraphic< AboutFunction > > AboutBoxView;
	
	
	static std::auto_ptr< Window > NewAboutBox()
	{
		Rect bounds = N::SetRect( 0, 0, 128, 128 );
		
		CenterWindowRect( bounds );
		
		NewWindowContext context( bounds, "\p" "Pedestal", true, Mac::noGrowDocProc );
		
		std::auto_ptr< Window > window( new UserWindow( context ) );
		
		if ( MacFeatures::Has_ColorQuickdraw() )
		{
			window->SetView( boost::intrusive_ptr< View >( new AboutBoxView( AboutFunction() ) ) );
		}
		else
		{
			window->SetView( boost::intrusive_ptr< View >( new EmptyView() ) );
		}
		
		return window;
	}
	
	
	void ShowAboutBox()
	{
		static UniqueWindowOwner gAboutBox( &NewAboutBox );
		
		gAboutBox.Show();
	}
	
}

