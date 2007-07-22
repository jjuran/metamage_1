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
			Io::StringPipe myInput;
			short myStartOfInput;
		
		public:
			bool eofReceived;
		
		public:
			ConsolePane( const Rect& bounds, Initializer init ) 
			: 
				Ped::Console( bounds, init ), 
				myStartOfInput( TextLength() ),
				eofReceived( false )
			{}
			
			Io::StringPipe& Input()  { return myInput; }
			
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
			
			GenieWindow( Ped::WindowClosure& closure );
			GenieWindow( Ped::WindowClosure& closure, ConstStr255Param title );
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
			
			void Open( ConstStr255Param title )
			{
				fWindow.reset( new GenieWindow( *this, title ) );
			}
			
			ConsoleTTYHandle* Salvage()  { DisassociateFromTerminal();  return Terminal(); }
	};
	
	
	class Console
	{
		private:
			N::Str63 fProgramName;
			TerminalWindowOwner fWindow;
			Io::StringPipe* myInput;
			std::string currentInput;
			int itsWindowSalvagePolicy;
			int itsLeaderWaitStatus;
			bool blockingMode;
		
		public:
			Console( ConsoleTTYHandle* terminal, ConstStr255Param name = "\p" "gterm" );
			
			~Console()  {}
			
			bool IsReadable() const;
			
			void OpenWindow( ConstStr255Param title = NULL );
			
			void SetTitle( ConstStr255Param title = NULL );
			
			int Read (       char* data, std::size_t byteCount );
			int Write( const char* data, std::size_t byteCount );
			
			const std::string& TTYName() const  { return fWindow.TTYName(); }
			
			GenieWindow* Window()  { return fWindow.Get(); }  // NULL if no window
			ConsolePane& Pane  ()  { return fWindow.Get()->SubView().ScrolledView(); }
			
			Io::StringPipe& Input()  { return Pane().Input(); }
			
			int GetWindowSalvagePolicy() const  { return itsWindowSalvagePolicy; }
			
			void SetWindowSalvagePolicy( int policy )  { itsWindowSalvagePolicy = policy; }
			
			void SetLeaderWaitStatus( int status )  { itsLeaderWaitStatus = status; }
			
			bool ShouldSalvageWindow() const;
			
			ConsoleTTYHandle* Salvage()  { return fWindow.Salvage(); }
	};
	
	class ConsolesOwner
	{
		public:
			typedef std::map< Console*, boost::shared_ptr< Console > > Map;
		
		private:
			Map map;
		
		public:
			const Map& GetMap() const  { return map; }
			
			Console* NewConsole( ConsoleTTYHandle* terminal );
			
			void CloseConsole( Console* console );
	};
	
	const ConsolesOwner::Map& GetConsoleMap();
	
	class IOHandle;
	
	boost::shared_ptr< IOHandle > NewConsoleDevice();
	
	void SpawnNewConsole( const FSSpec& program );
	void SpawnNewConsole();
	
	Console* NewConsole( ConsoleTTYHandle* terminal );
	
	void CloseConsole( Console* console );
	
}

#endif

