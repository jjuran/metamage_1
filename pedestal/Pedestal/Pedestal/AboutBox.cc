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
		Rect bounds = N::OffsetRect( N::SetRect( 0, 0, 128, 128 ),
		                             300,
		                             200 );
		
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

