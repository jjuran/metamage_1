/*	===========
 *	AboutBox.cc
 *	===========
 */

#include "Pedestal/AboutBox.hh"

// Pedestal
#include "Pedestal/GeneratedGraphic.hh"
#include "Pedestal/GraphicView.hh"
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
	
	AboutBoxOwner::~AboutBoxOwner()
	{
	}
	
	std::auto_ptr< Window > AboutBoxOwner::New()
	{
		Rect bounds = N::OffsetRect( N::SetRect( 0, 0, 128, 128 ),
		                             300,
		                             200 );
		
		NewWindowContext context( bounds, "\p" "Pedestal" );
		
		std::auto_ptr< Window > window( new UserWindow( context, N::noGrowDocProc ) );
		
		window->SetView( std::auto_ptr< View >( new AboutBoxView( AboutFunction() ) ) );
		
		return window;
	}
	
}

