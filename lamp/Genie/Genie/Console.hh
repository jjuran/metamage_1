/*	===================
 *	GenieConsoleTerm.hh
 *	===================
 */

#pragma once

// Standard C++
#include <list>
#include <memory>
#include <string>

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
#include "Genie/Process.hh"


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
	
	class GenieWindow : public Ped::Window< Ped::Scroller< ConsolePane, Ped::kVerticalScrollbar > >
	{
		public:
			typedef Ped::Window< Ped::Scroller< ConsolePane, Ped::kVerticalScrollbar > > Base;
			
			GenieWindow( Ped::WindowClosure& closure );
			GenieWindow( Ped::WindowClosure& closure, ConstStr255Param title );
	};
	
	
	class HangupWindowClosure : public Ped::WindowClosure
	{
		private:
			CharacterDevice* fTerminal;
		
		public:
			HangupWindowClosure( CharacterDevice* terminal ) : fTerminal( terminal )  {}
			
			bool RequestWindowClosure( N::WindowRef )
			{
				gProcessTable.SendSignalToProcessesControlledByTerminal( SIGHUP, fTerminal );
				
				// Assuming the window does get shut, it hasn't happened yet
				return false;
			}
	};
	
	class TerminalWindowOwner : private HangupWindowClosure
	{
		typedef std::auto_ptr< GenieWindow > WindowStorage;
		
		protected:
			WindowStorage         fWindow;
		
		public:
			TerminalWindowOwner( CharacterDevice* terminal ) : HangupWindowClosure( terminal )  {}
			
			GenieWindow const* Get() const  { return fWindow.get(); }
			GenieWindow      * Get()        { return fWindow.get(); }
			
			void Open( ConstStr255Param title )
			{
				fWindow.reset( new GenieWindow( *this, title ) );
			}
	};
	
	
	class Console
	{
		private:
			N::Str63 fProgramName;
			TerminalWindowOwner fWindow;
			Io::StringPipe* myInput;
			std::string currentInput;
			//bool stayOpen;
			bool blockingMode;
		
		public:
			Console( CharacterDevice* terminal, ConstStr255Param name = "\p" "gterm" );
			
			~Console()  {}
			
			void SetBlockingMode( bool mode )  { blockingMode = mode; }
			
			int Read (       char* data, std::size_t byteCount );
			int Write( const char* data, std::size_t byteCount );
			
			GenieWindow* Window()  { return fWindow.Get(); }  // NULL if no window
			ConsolePane& Pane  ()  { return fWindow.Get()->SubView().ScrollView(); }
			
			Io::StringPipe& Input()  { return Pane().Input(); }
	};
	
	void SpawnNewConsole( const FSSpec& program );
	void SpawnNewConsole();
	
	Console* NewConsole( CharacterDevice* terminal );
	
	void CloseConsole( Console* console );
	
}

