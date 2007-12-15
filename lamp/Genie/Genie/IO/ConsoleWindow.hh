/*	=====================
 *	Genie/ConsolWindow.hh
 *	=====================
 */

#ifndef GENIE_CONSOLEWINDOW_HH
#define GENIE_CONSOLEWINDOW_HH

// Standard C++
#include <memory>
#include <string>

// boost
#include <boost/shared_ptr.hpp>

// Io
#include "Io/TextInput.hh"

// Pedestal
#include "Pedestal/Console.hh"
#include "Pedestal/Scroller.hh"
#include "Pedestal/Window.hh"

// Genie
#include "Genie/IO/Window.hh"


namespace Genie
{
	
	typedef std::size_t ConsoleID;
	
	class ConsolePane : public Pedestal::Console
	{
		private:
			ConsoleID        itsConsoleID;
			Io::StringPipe&  itsInput;
			short            itsStartOfInput;
			bool             itHasReceivedEOF;
		
		public:
			struct Initializer : public Pedestal::Console::Initializer
			{
				ConsoleID        id;
				Io::StringPipe&  input;
				
				Initializer( ConsoleID id, Io::StringPipe& in ) : id( id ), input( in )  {}
			};
		
		public:
			ConsolePane( const Rect&         bounds,
			             const Initializer&  init   ) : Pedestal::Console( bounds, init ),
			                                            itsConsoleID     ( init.id      ),
			                                            itsInput         ( init.input   ),
			                                            itsStartOfInput  ( TextLength() ),
			                                            itHasReceivedEOF ( false        )
			{
			}
			
			bool CheckEOF();
			
			int WriteChars( const char* data, unsigned int byteCount );
			
			void MouseDown( const EventRecord& event );
			bool KeyDown  ( const EventRecord& event );
			
			bool UserCommand( Pedestal::MenuItemCode code );
			
			void Paste();
	};
	
	
	class ConsoleWindow : public Pedestal::Window< Pedestal::Scroller< ConsolePane, Pedestal::kLiveFeedbackVariant > >,
	                      public WindowHandle
	{
		private:
			Io::StringPipe itsInput;
		
		public:
			typedef Pedestal::Window< Pedestal::Scroller< ConsolePane, Pedestal::kLiveFeedbackVariant > > Base;
			
			ConsoleWindow( ConsoleID id, const std::string& name );
			
			~ConsoleWindow();
			
			Pedestal::WindowCore& GetWindowCore() { return *this; }
			
			Nitrogen::WindowRef GetWindowRef() const  { return Get(); }
			
			bool IsReadyForInput();
			
			std::string ReadInput()  { return itsInput.Read(); }
			
			int Write( const char* data, std::size_t byteCount );
	};
	
}

#endif

