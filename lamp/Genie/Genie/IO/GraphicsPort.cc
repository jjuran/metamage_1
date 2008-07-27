/*	===============
 *	GraphicsPort.cc
 *	===============
 */

#include "Genie/IO/GraphicsPort.hh"

// Lamp
#include "lamp/grafio.h"

// Nucleus
#include "Nucleus/Saved.h"

// Io
#include "io/io.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace Ped = Pedestal;
	
	
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
	
	
	GraphicsWindow::GraphicsWindow( TerminalID          id,
	                                const std::string&  name ) : Base( Ped::NewWindowContext( MakeWindowRect(),
	                                                                                          "\p" "Graphics",
	                                                                                          true ) ),
	                                                             WindowHandle( name )
	{
		SetCloseHandler ( GetDynamicWindowCloseHandler < GraphicsWindow >( id ) );
		SetResizeHandler( GetDynamicWindowResizeHandler< GraphicsWindow >( id ) );
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
			
			case GIOCFLUSHRECT:
				{
					N::CGrafPtr port = N::GetWindowPort( GetWindowRef() );
					
					Rect bounds = argp ? *(Rect*) argp : N::GetPortBounds( port );
					
					// Lock pixels
					NN::Saved< N::PixelsState_Value > savedPixelsState( pix );
					
					N::LockPixels( pix );
					
					// Copy to dest
					N::CopyBits( N::GetPortBitMapForCopyBits( gworld ),
					             N::GetPortBitMapForCopyBits( port   ),
					             bounds,
					             bounds,
					             N::srcCopy );
					
					// Unlock pixels
				}
				
				break;
			
			case GIOCSBGCOLOR:
				if ( const RGBColor* bg = (const RGBColor*) argp )
				{
					NN::Saved< N::GWorld_Value > savedGWorld;
					
					N::SetGWorld( gworld );
					
					N::RGBBackColor( *bg );
				}
				
				break;
			
			default:
				WindowHandle::IOCtl( request, argp );
				break;
		}
	}
	
}

