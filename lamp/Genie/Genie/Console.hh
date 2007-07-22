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


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace Ped = Pedestal;
	
	
	class ConsolePane : public Ped::Console
	{
		private:
			Io::StringPipe& itsInput;
			short myStartOfInput;
		
		public:
			struct Initializer : public Ped::Console::Initializer
			{
				Io::StringPipe& input;
				
				Initializer( Io::StringPipe& in ) : input( in )  {}
			};
			
			bool eofReceived;
		
		public:
			ConsolePane( const Rect& bounds, const Initializer& init ) 
			: 
				Ped::Console( bounds, init ),
				itsInput( init.input ),
				myStartOfInput( TextLength() ),
				eofReceived( false )
			{}
			
			int WriteChars( const char* data, unsigned int byteCount );
			
			void MouseDown( const EventRecord& event );
			bool KeyDown  ( const EventRecord& event );
			
			bool UserCommand( Ped::MenuItemCode code );
			
			void Paste();
	};
	
	class GenieWindow : public Ped::Window< Ped::Scroller< ConsolePane, Ped::kLiveFeedbackVariant > >
	{
		public:
			typedef Ped::Window< Ped::Scroller< ConsolePane, Ped::kLiveFeedbackVariant > > Base;
			
			GenieWindow( Ped::WindowClosure& closure, ConstStr255Param title, Io::StringPipe& input );
	};
	
	
	class ConsoleTTYHandle;
	
	class ConsoleWindowClosure : public Ped::WindowClosure
	{
		private:
			ConsoleTTYHandle* itsTerminal;
			bool itHasBeenRequested;
			bool itHasDisassociated;
		
		protected:
			// prevent slicing
			~ConsoleWindowClosure()  {}
		
		public:
			ConsoleWindowClosure( ConsoleTTYHandle* terminal ) : itsTerminal( terminal ),
			                                                     itHasBeenRequested(),
			                                                     itHasDisassociated()
			{
			}
			
			ConsoleTTYHandle* Terminal() const  { return itsTerminal; }
			
			const std::string& TTYName() const;
			
			bool RequestWindowClosure( N::WindowRef );
			
			bool ClosureHasBeenRequested() const  { return itHasBeenRequested; }
			
			void DisassociateFromTerminal()  { itHasDisassociated = true; }
	};
	
	class TerminalWindowOwner : public ConsoleWindowClosure
	{
		typedef std::auto_ptr< GenieWindow > WindowStorage;
		
		protected:
			WindowStorage         fWindow;
		
		public:
			TerminalWindowOwner( ConsoleTTYHandle* terminal ) : ConsoleWindowClosure( terminal )  {}
			
			GenieWindow const* Get() const  { return fWindow.get(); }
			GenieWindow      * Get()        { return fWindow.get(); }
			
			void Open( ConstStr255Param title, Io::StringPipe& input )
			{
				fWindow.reset( new GenieWindow( *this, title, input ) );
			}
			
			ConsoleTTYHandle* Salvage()  { DisassociateFromTerminal();  return Terminal(); }
	};
	
	
	class Console
	{
		private:
			TerminalWindowOwner fWindow;
			Io::StringPipe itsInput;
			std::string currentInput;
			int itsWindowSalvagePolicy;
			int itsLeaderWaitStatus;
			bool blockingMode;
		
		public:
			Console( ConsoleTTYHandle* terminal );
			
			~Console()  {}
			
			bool IsReadable() const;
			
			void OpenWindow( ConstStr255Param title = NULL );
			
			void SetTitle( ConstStr255Param title = NULL );
			
			int Read (       char* data, std::size_t byteCount );
			int Write( const char* data, std::size_t byteCount );
			
			const std::string& TTYName() const  { return fWindow.TTYName(); }
			
			GenieWindow* Window()  { return fWindow.Get(); }  // NULL if no window
			ConsolePane& Pane  ()  { return fWindow.Get()->SubView().ScrolledView(); }
			
			int GetWindowSalvagePolicy() const  { return itsWindowSalvagePolicy; }
			
			void SetWindowSalvagePolicy( int policy )  { itsWindowSalvagePolicy = policy; }
			
			void SetLeaderWaitStatus( int status )  { itsLeaderWaitStatus = status; }
			
			bool ShouldSalvageWindow() const;
			
			ConsoleTTYHandle* Salvage();
	};
	
	typedef std::map< Console*, boost::shared_ptr< Console > > ConsoleMap;
	
	const ConsoleMap& GetConsoleMap();
	
	class IOHandle;
	
	boost::shared_ptr< IOHandle > NewConsoleDevice();
	
	void SpawnNewConsole( const FSSpec& program );
	void SpawnNewConsole();
	
	Console* NewConsole( ConsoleTTYHandle* terminal );
	
	void CloseConsole( Console* console );
	
}

#endif

