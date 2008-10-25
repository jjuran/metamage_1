/*	===============
 *	GraphicsPort.hh
 *	===============
 */

#ifndef GENIE_IO_GRAPHICSPORT_HH
#define GENIE_IO_GRAPHICSPORT_HH

// Genie
#include "Genie/IO/Window.hh"


namespace Genie
{
	
	class GraphicsWindow : public WindowHandle
	{
		public:
			static const char* PathPrefix()  { return "/sys/set/gfx/"; }
			
			GraphicsWindow( TerminalID id, const std::string& name );
			
			~GraphicsWindow();
			
			void IOCtl( unsigned long request, int* argp );
	};
	
}

#endif

