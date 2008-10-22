/*	===============
 *	GraphicsPort.hh
 *	===============
 */

#ifndef GENIE_IO_GRAPHICSPORT_HH
#define GENIE_IO_GRAPHICSPORT_HH

// Pedestal
#include "Pedestal/UserWindow.hh"

// Genie
#include "Genie/IO/Window.hh"


namespace Genie
{
	
	class GraphicsWindow : public Pedestal::UserWindow,
	                       public WindowHandle
	{
		private:
			
		
		public:
			typedef Pedestal::UserWindow Base;
			
			static const char* PathPrefix()  { return "/sys/set/gfx/"; }
			
			GraphicsWindow( TerminalID id, const std::string& name );
			
			~GraphicsWindow();
			
			void IOCtl( unsigned long request, int* argp );
			
			Pedestal::WindowCore& GetWindowCore() { return *this; }
			
			Nitrogen::WindowRef GetWindowRef() const  { return Get(); }
	};
	
}

#endif

