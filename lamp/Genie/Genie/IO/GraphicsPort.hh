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
#include "Genie/IO/Window.hh"


namespace Genie
{
	
	class GraphicsWindow : public Pedestal::Window< Pedestal::GWorldView >,
	                       public WindowHandle
	{
		private:
			
		
		public:
			typedef Pedestal::Window< Pedestal::GWorldView > Base;
			
			GraphicsWindow( const std::string& name );
			
			~GraphicsWindow();
			
			void IOCtl( unsigned long request, int* argp );
			
			Pedestal::WindowCore& GetWindowCore() { return *this; }
			
			Nitrogen::WindowRef GetWindowRef() const  { return Get(); }
	};
	
	
	boost::shared_ptr< IOHandle > NewGraphicsPort();
	
}

#endif

