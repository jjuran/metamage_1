/*	=====================
 *	Genie/ConsolWindow.hh
 *	=====================
 */

#ifndef GENIE_CONSOLEWINDOW_HH
#define GENIE_CONSOLEWINDOW_HH

// Standard C++
#include <string>

// Genie
#include "Genie/IO/Window.hh"


namespace Genie
{
	
	typedef std::size_t ConsoleID;
	
	
	class ConsoleWindow : public WindowHandle
	{
		public:
			ConsoleWindow( ConsoleID id, const std::string& name );
			
			~ConsoleWindow();
			
			void IOCtl( unsigned long request, int* argp );
			
			bool IsReadyForInput();
			
			std::string ReadInput();
			
			int Write( const char* data, std::size_t byteCount );
	};
	
}

#endif

