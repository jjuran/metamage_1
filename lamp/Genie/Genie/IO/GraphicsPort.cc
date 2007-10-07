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
		boost::shared_ptr< IOHandle > window( new GraphicsWindow( "\p" "Graphics" ) );
		
		return window;
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
	
	class NullCloseHandler : public Ped::WindowCloseHandler
	{
		public:
			void operator()( N::WindowRef window ) const
			{
			}
	};
	
	static boost::shared_ptr< Ped::WindowCloseHandler > NewBufferCloseHandler()
	{
		return boost::shared_ptr< Ped::WindowCloseHandler >( new NullCloseHandler() );
	}
	
	
	GraphicsWindow::GraphicsWindow( ConstStr255Param title ) : Base( Ped::NewWindowContext( MakeWindowRect(),
	                                                                                        title,
	                                                                                        true ),
	                                                                 NewBufferCloseHandler() )
	{
	}
	
	GraphicsWindow::~GraphicsWindow()
	{
	}
	
}

