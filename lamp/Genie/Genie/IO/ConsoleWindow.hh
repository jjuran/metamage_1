/*	=====================
 *	Genie/ConsolWindow.hh
 *	=====================
 */

#ifndef GENIE_CONSOLEWINDOW_HH
#define GENIE_CONSOLEWINDOW_HH

// Standard C++
#include <string>

// Io
#include "Io/TextInput.hh"

// Pedestal
#include "Pedestal/UserWindow.hh"

// Genie
#include "Genie/IO/Window.hh"


namespace Genie
{
	
	typedef std::size_t ConsoleID;
	
	
	class ConsoleWindow : public Pedestal::UserWindow,
	                      public WindowHandle
	{
		private:
			Io::StringPipe itsInput;
		
		public:
			typedef Pedestal::UserWindow Base;
			
			ConsoleWindow( ConsoleID id, const std::string& name );
			
			~ConsoleWindow();
			
			void IOCtl( unsigned long request, int* argp );
			
			Pedestal::WindowCore& GetWindowCore() { return *this; }
			
			Nitrogen::WindowRef GetWindowRef() const  { return Get(); }
			
			bool IsReadyForInput();
			
			std::string ReadInput()  { return itsInput.Read(); }
			
			int Write( const char* data, std::size_t byteCount );
	};
	
}

#endif

