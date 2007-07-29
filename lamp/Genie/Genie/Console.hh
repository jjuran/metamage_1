/*	================
 *	Genie/Console.hh
 *	================
 */

#ifndef GENIE_CONSOLE_HH
#define GENIE_CONSOLE_HH

// Standard C++
#include <list>
#include <map>
#include <memory>
#include <string>

// boost
#include <boost/shared_ptr.hpp>

// Nitrogen
#include "Nitrogen/Files.h"

// Io
#include "Io/TextInput.hh"

// Pedestal
#include "Pedestal/Console.hh"
#include "Pedestal/Scroller.hh"
#include "Pedestal/Window.hh"
#include "Pedestal/View.hh"

// Genie
#include "Genie/IO/ConsoleTTY.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace Ped = Pedestal;
	
	
	class ConsolePane : public Ped::Console
	{
		private:
			ConsoleID        itsConsoleID;
			Io::StringPipe&  itsInput;
			short            itsStartOfInput;
			bool             itHasReceivedEOF;
		
		public:
			struct Initializer : public Ped::Console::Initializer
			{
				ConsoleID        id;
				Io::StringPipe&  input;
				
				Initializer( ConsoleID id, Io::StringPipe& in ) : id( id ), input( in )  {}
			};
		
		public:
			ConsolePane( const Rect&         bounds,
			             const Initializer&  init   ) : Ped::Console    ( bounds, init ),
			                                            itsConsoleID    ( init.id      ),
			                                            itsInput        ( init.input   ),
			                                            itsStartOfInput ( TextLength() ),
			                                            itHasReceivedEOF( false        )
			{
			}
			
			void CheckEOF();
			
			int WriteChars( const char* data, unsigned int byteCount );
			
			void MouseDown( const EventRecord& event );
			bool KeyDown  ( const EventRecord& event );
			
			bool UserCommand( Ped::MenuItemCode code );
			
			void Paste();
	};
	
	
	class ConsoleWindow : public Ped::Window< Ped::Scroller< ConsolePane, Ped::kLiveFeedbackVariant > >
	{
		private:
			Io::StringPipe itsInput;
		
		public:
			typedef Ped::Window< Ped::Scroller< ConsolePane, Ped::kLiveFeedbackVariant > > Base;
			
			ConsoleWindow( ConsoleID id, ConstStr255Param title );
			
			bool IsReadyForInput();
			
			std::string ReadInput()  { return itsInput.Read(); }
			
			int Write( const char* data, std::size_t byteCount );
	};
	
	
	void SpawnNewConsole( const FSSpec& program );
	void SpawnNewConsole();
	
	void CloseConsole( const boost::shared_ptr< ConsoleWindow >& console );
	
	void SalvageConsole( const boost::shared_ptr< ConsoleWindow >& console );
	
}

#endif

