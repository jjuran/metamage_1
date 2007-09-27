/*	===============
 *	GraphicsPort.hh
 *	===============
 */

#ifndef GENIE_IO_GRAPHICSPORT_HH
#define GENIE_IO_GRAPHICSPORT_HH

// boost
#include <boost/shared_ptr.hpp>

// Pedestal
#include "Pedestal/GWorldView.hh"
#include "Pedestal/Window.hh"

// Genie
//#include "Genie/IO/RegularFile.hh"
#include "Genie/IO/Window.hh"


namespace Genie
{
	
	namespace Ped = Pedestal;
	
	
	class GraphicsWindow : public Ped::Window< Ped::GWorldView >,
	                       public WindowHandle
	{
		private:
			
		
		public:
			typedef Ped::Window< Ped::GWorldView > Base;
			
			GraphicsWindow( ConstStr255Param title );
			
			~GraphicsWindow();
			
			Nitrogen::WindowRef GetWindowRef() const  { return Get(); }
	};
	
	
	boost::shared_ptr< IOHandle > NewGraphicsPort();
	
}

#endif

