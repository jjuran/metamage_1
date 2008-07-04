/*	===============
 *	GraphicsPort.cc
 *	===============
 */

#include "Genie/IO/GraphicsPort.hh"

// Lamp
#include "lamp/grafio.h"

// Io
#include "io/io.hh"

// POSeven
#include "POSeven/Errno.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace Ped = Pedestal;
	
	
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
	
	void GraphicsWindow::IOCtl( unsigned long request, int* argp )
	{
		N::GWorldPtr gworld = SubView().Get();
		
		PixMapHandle pix = N::GetGWorldPixMap( gworld );
		
		switch ( request )
		{
			case GIOCGBASEADDR:
				if ( void** baseAddr = (void**) argp )
				{
					*baseAddr = *(void**) pix[0]->baseAddr;
				}
				
				break;
			
			case GIOCGROWBYTES:
				if ( unsigned short* rowBytes = (unsigned short*) argp )
				{
					*rowBytes = pix[0]->rowBytes & 0x3fff;
				}
				
				break;
			
			case GIOCGNROWS:
				if ( unsigned short* rows = (unsigned short*) argp )
				{
					*rows = pix[0]->bounds.bottom - pix[0]->bounds.top;
				}
				
				break;
			
			case GIOCGDEPTH:
				if ( unsigned char* depth = (unsigned char*) argp )
				{
					*depth = pix[0]->pixelSize;
				}
				
				break;
			
			default:
				WindowHandle::IOCtl( request, argp );
				break;
		}
	}
	
}

