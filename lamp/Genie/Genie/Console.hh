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
			Io::StringPipe&  itsInput;
			short            itsStartOfInput;
			bool             itHasReceivedEOF;
		
		public:
			struct Initializer : public Ped::Console::Initializer
			{
				Io::StringPipe& input;
				
				Initializer( Io::StringPipe& in ) : input( in )  {}
			};
		
		public:
			ConsolePane( const Rect&         bounds,
			             const Initializer&  init   ) : Ped::Console    ( bounds, init ),
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
	
	class GenieWindow : public Ped::Window< Ped::Scroller< ConsolePane, Ped::kLiveFeedbackVariant > >
	{
		private:
			Io::StringPipe itsInput;
		
		public:
			typedef Ped::Window< Ped::Scroller< ConsolePane, Ped::kLiveFeedbackVariant > > Base;
			
			GenieWindow( Ped::WindowClosure& closure, ConstStr255Param title );
			
			Io::StringPipe const& Input() const  { return itsInput; }
			Io::StringPipe      & Input()        { return itsInput; }
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
				fWindow.reset( new GenieWindow( *this, title) );
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
			
			~Console();
			
			bool IsReadable() const;
			
			void OpenWindow( ConstStr255Param title = NULL );
			
			void SetTitle( ConstStr255Param title = NULL );
			
			int Read (       char* data, std::size_t byteCount );
			int Write( const char* data, std::size_t byteCount );
			
			const std::string& TTYName() const  { return fWindow.TTYName(); }
			
			GenieWindow const* Window() const  { return fWindow.Get(); }  // NULL if no window
			GenieWindow      * Window()        { return fWindow.Get(); }  // NULL if no window
			
			ConsolePane const& Pane  () const  { return fWindow.Get()->SubView().ScrolledView(); }
			ConsolePane      & Pane  ()        { return fWindow.Get()->SubView().ScrolledView(); }
			
			int GetWindowSalvagePolicy() const  { return itsWindowSalvagePolicy; }
			
			void SetWindowSalvagePolicy( int policy )  { itsWindowSalvagePolicy = policy; }
			
			void SetLeaderWaitStatus( int status )  { itsLeaderWaitStatus = status; }
			
			bool ShouldSalvageWindow() const;
			
			ConsoleTTYHandle* Salvage();
	};
	
	typedef std::map< Console*, boost::weak_ptr< Console > > ConsoleMap;
	
	const ConsoleMap& GetConsoleMap();
	
	class IOHandle;
	
	boost::shared_ptr< IOHandle > NewConsoleDevice();
	
	void SpawnNewConsole( const FSSpec& program );
	void SpawnNewConsole();
	
	boost::shared_ptr< Console > NewConsole( ConsoleTTYHandle* terminal );
	
	void CloseConsole( const boost::shared_ptr< Console >& console );
	
}

#endif

