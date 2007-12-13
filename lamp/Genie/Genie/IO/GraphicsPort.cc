/*	===============
 *	GraphicsPort.cc
 *	===============
 */

#include "Genie/IO/GraphicsPort.hh"

// Io
#include "io/io.hh"

// POSeven
#include "POSeven/Errno.hh"

// Genie
#include "Genie/Yield.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	
	boost::shared_ptr< IOHandle > NewGraphicsPort()
	{
		GraphicsWindow* window = new GraphicsWindow( "Graphics" );
		
		boost::shared_ptr< IOHandle > handle( static_cast< IOHandle* >( window ) );
		
		AddWindowToMap( window->Get(), handle );
		
		return handle;
	}
	
	
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
	
	
	GraphicsWindow::GraphicsWindow( const std::string& name ) : Base( Ped::NewWindowContext( MakeWindowRect(),
	                                                                                         N::Str255( name ),
	                                                                                         true ),
	                                                                  GetTerminalCloseHandler() ),
	                                                            WindowHandle( name )
	{
	}
	
	GraphicsWindow::~GraphicsWindow()
	{
	}
	
}

