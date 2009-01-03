// SystemConsole.cc

#include "Genie/SystemConsole.hh"

// Pedestal
#include "Pedestal/Console.hh"
#include "Pedestal/Scroller.hh"
#include "Pedestal/UniqueWindowOwner.hh"
#include "Pedestal/UserWindow.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace Ped = Pedestal;
	
	
	typedef Ped::TEScrollFrame< true > SystemConsoleView;
	
	
	class SystemConsoleOwner : public Ped::UniqueWindowOwner
	{
		private:
			static std::auto_ptr< Ped::Window > New();
		
		public:
			SystemConsoleOwner() : Ped::UniqueWindowOwner( &New )
			{
			}
			
			static int WriteToSystemConsole( const char* data, std::size_t byteCount );
	};
	
	
	static Rect MakeWindowRect()
	{
		BitMap screenBits = N::GetQDGlobalsScreenBits();
		
		Rect rect = N::SetRect(0, 18, 2*4+6*80+16, 18+2*4+11*25+16);
		short mbarHeight = ::GetMBarHeight();
		short vMargin = (screenBits.bounds.bottom - rect.bottom) - mbarHeight;
		short hMargin = (screenBits.bounds.right  - rect.right);
		
		return N::OffsetRect( rect,
		                      hMargin / 2,
		                      mbarHeight + vMargin / 3 );
	}
	
	static inline std::auto_ptr< Ped::View > MakeView()
	{
		Rect scroller_bounds = MakeWindowRect();
		
		Rect subview_bounds = Pedestal::ScrollBounds< true, false >( scroller_bounds );
		
		SystemConsoleView* scrollframe = NULL;
		
		std::auto_ptr< Ped::View > view( scrollframe = new SystemConsoleView( scroller_bounds ) );
		
		std::auto_ptr< Ped::View > subview( new Ped::Console( subview_bounds ) );
		
		scrollframe->SetSubView( subview );
		
		return view;
	}
	
	std::auto_ptr< Ped::Window > SystemConsoleOwner::New()
	{
		Rect bounds = MakeWindowRect();
		
		Ped::NewWindowContext context( bounds, "\p" "System Console" );
		
		std::auto_ptr< Ped::Window > window( new Ped::UserWindow( context, N::documentProc ) );
		
		window->SetView( MakeView() );
		
		return window;
	}
	
	int SystemConsoleOwner::WriteToSystemConsole( const char* data, std::size_t byteCount )
	{
		static SystemConsoleOwner gSystemConsoleOwner;
		
		gSystemConsoleOwner.Show();
		
		SystemConsoleView& view = gSystemConsoleOwner.Get().SubView< SystemConsoleView >();
		
		static_cast< Ped::Console& >( view.GetSubView() ).WriteChars( data, byteCount );
		
		view.UpdateScrollbars();
		
		return byteCount;
	}
	
	int WriteToSystemConsole( const char* data, std::size_t byteCount )
	{
		return SystemConsoleOwner::WriteToSystemConsole( data, byteCount );
	}
	
}

