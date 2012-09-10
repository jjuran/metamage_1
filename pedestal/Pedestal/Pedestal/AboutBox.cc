/*	===========
 *	AboutBox.cc
 *	===========
 */

#include "Pedestal/AboutBox.hh"

// Nitrogen
#include "Nitrogen/Icons.hh"
#include "Nitrogen/Quickdraw.hh"

// Pedestal
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
	
	class AboutBoxView : public View
	{
		public:
			void Draw( const Rect& bounds, bool erasing );
	};
	
	void AboutBoxView::Draw( const Rect& bounds, bool erasing )
	{
		if ( erasing )
		{
			N::EraseRect( bounds );
		}
		
		N::PlotIconID( bounds,
		               N::IconAlignmentType(),
		               N::IconTransformType(),
		               N::ResID( 128 ) );
	}
	
	
	static std::auto_ptr< Window > NewAboutBox()
	{
		Rect bounds = N::SetRect( 0, 0, 128, 128 );
		
		CenterWindowRect( bounds );
		
		NewWindowContext context( bounds, "\p" "Pedestal", true, Mac::noGrowDocProc );
		
		std::auto_ptr< Window > window( new UserWindow( context ) );
		
		window->SetView( boost::intrusive_ptr< View >( new AboutBoxView() ) );
		
		return window;
	}
	
	
	void ShowAboutBox()
	{
		static UniqueWindowOwner gAboutBox( &NewAboutBox );
		
		gAboutBox.Show();
	}
	
}

