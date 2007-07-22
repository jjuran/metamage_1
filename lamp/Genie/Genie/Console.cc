/*	===================
 *	GenieConsoleTerm.cc
 *	===================
 */

#include "Genie/Console.hh"

// Mac OS Universal Headers
#include <Menus.h>

// Standard C++ library
#include <string>
#include <vector>

// Lamp
#include <lamp/winio.h>

// Nucleus
#include "Nucleus/NAssert.h"

// Io
#include "io/io.hh"

// Nitrogen
#include "Nitrogen/Processes.h"
#include "Nitrogen/Sound.h"

// Genie
#include "Genie/FileSystem/ResolvePathname.hh"
#include "Genie/IO/ConsoleTTY.hh"
#include "Genie/Process.hh"
#include "Genie/Yield.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
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
	
	
	void ConsolePane::CheckEOF()
	{
		if ( itHasReceivedEOF )
		{
			itHasReceivedEOF = false;
			
			throw io::end_of_input();
		}
	}
	
	int ConsolePane::WriteChars( const char* data, unsigned int byteCount )
	{
		int written = Ped::Console::WriteChars( data, byteCount );
		itsStartOfInput = TextLength();
		return written;
	}
	
	void ConsolePane::MouseDown( const EventRecord& event )
	{
		if ( event.modifiers & cmdKey )
		{
			//DebugBeep();
		}
		else
		{
			Ped::Console::MouseDown( event );
		}
	}
	
	struct TESelection
	{
		short start, end;
	};
	
	static TESelection GetTESelection( TEHandle hTE )
	{
		TESelection selection;
		
		selection.start = (**hTE).selStart;
		selection.end   = (**hTE).selEnd;
		
		return selection;
	}
	
	static std::string GetHandleSubString( ::Handle     h,
	                                       std::size_t  offset,
	                                       std::size_t  length )
	{
		std::string result( length, '\0' );
		
		const char* start = *h + offset;
		
		std::copy( start,
		           start + length,
		           result.begin() );
		
		return result;
	}
	
	static unsigned char Cntrl( unsigned char c )
	{
		return c & 0x1F;
	}
	
	bool ConsolePane::KeyDown( const EventRecord& event )
	{
		char c = event.message & charCodeMask;
		
		TESelection selection = GetTESelection( Get() );
		
		short& start = selection.start;
		short& end   = selection.end;
		
		if ( c == kEnterCharCode )
		{
			short cmdLen = end - start;
			
			if ( cmdLen > 0 )
			{
				std::string command = GetHandleSubString( TextHandle(), start, cmdLen );
				
				std::replace( command.begin(),
				              command.end(),
				              '\r',
				              '\n' );
				
				command += '\n';
				
				itsInput.Write( command );
			}
			
			return true;
		}
		
		start = std::max( itsStartOfInput, start );
		end   = std::max( start, end );
		
		bool textIsSelected = start != end;
		bool atStartOfInput = start == itsStartOfInput;
		
		if ( event.modifiers & controlKey  &&  c < 0x20 )
		{
			char cntrl = c | 0x40;
			
			switch ( cntrl )
			{
				case 'A':
					start = end = itsStartOfInput;
					
					SetSelection( start, end );
					break;
				
				case 'E':
					start = end = TextLength();
					
					SetSelection( start, end );
					break;
				
				case 'D':
					if ( TextLength() - itsStartOfInput <= 0 )
					{
						itHasReceivedEOF = true;
					}
					else
					{
						N::SysBeep();
					}
					
					break;
			}
			
			return true;
		}
		
		switch ( c )
		{
			case kLeftArrowCharCode:
			case kRightArrowCharCode:
				
				if ( textIsSelected )
				{
					// Put the insertion point at the beginning or end of the selection.
					( c == kLeftArrowCharCode ) ? ( end = start )
					                            : ( start = end );
				}
				else
				{
					// The position delta is 1 for right arrow and -1 for left arrow,
					// except that it's zero if we're at the start of the input area 
					// going left.  We don't have to detect end of buffer going right,
					// because TE already handles this.
					short delta = ( c == kRightArrowCharCode ) ? 1
					                                           : ( atStartOfInput ? 0
					                                                              : -1 );
					start = end += delta;
				}
				
				SetSelection( start, end );
				break;
			
			case kUpArrowCharCode:
			case kDownArrowCharCode:
				break;
			
			case kReturnCharCode:
				// Always jump to end of buffer
				SetSelection( 32767, 32767 );
				
				// Insert the return char
				Ped::TEView::KeyDown( event );
				
				// No constructors directly in a switch block
				{
					short cmdLen = std::max( TextLength() - itsStartOfInput, 0 );
					//ASSERT( cmdLen >= 0 );
					
					std::string command = GetHandleSubString( TextHandle(), itsStartOfInput, cmdLen );
					
					std::replace( command.begin(),
					              command.end(),
					              '\r',
					              '\n' );
					
					itsInput.Write( command );
				}
				
				itsStartOfInput = TextLength();
				break;
			
			default:
				SetSelection( start, end );
				
				// Backspace or left arrow on a selection doesn't modify start --
				// only if there's an insertion point are we really going left.
				// If so, and we're at the start of input, ignore the event.
				bool goingLeft = ( c == kBackspaceCharCode )  ||  ( c == kLeftArrowCharCode );
				bool reallyGoingLeft = goingLeft && !textIsSelected;
				bool ignoring = reallyGoingLeft && atStartOfInput;
				
				if ( !ignoring )
				{
					Ped::TEView::KeyDown( event );
				}
				break;
		}
		
		return true;
	}
	
	bool ConsolePane::UserCommand( Ped::MenuItemCode code )
	{
		if ( code == 'pste' )
		{
			Paste();
			return true;
		}
		else
		{
			return Console::UserCommand( code );
		}
	}
	
	void ConsolePane::Paste()
	{
		// copied from DoKey -- refactor
		TESelection selection = GetTESelection( Get() );
		
		selection.start = std::max( itsStartOfInput, selection.start );
		selection.end   = std::max( selection.start, selection.end );
		
		SetSelection( selection.start, selection.end );
		
		Console::Paste();
	}
	
	
	static Rect MakeWindowRect()
	{
		BitMap screenBits = N::GetQDGlobalsScreenBits();
		
		Rect rect = N::SetRect(0, 18, 2*4+6*80+16, 18+2*4+11*25+16);
		short mbarHeight = ::GetMBarHeight();
		short vMargin = (screenBits.bounds.bottom - rect.bottom) - mbarHeight;
		short hMargin = (screenBits.bounds.right  - rect.right);
		
		return N::OffsetRect( rect,
		                      hMargin / 2,
		                      mbarHeight + vMargin / 3 );
	}
	
	
	class ConsoleWindow : public Ped::Window< Ped::Scroller< ConsolePane, Ped::kLiveFeedbackVariant > >
	{
		private:
			Io::StringPipe itsInput;
		
		public:
			typedef Ped::Window< Ped::Scroller< ConsolePane, Ped::kLiveFeedbackVariant > > Base;
			
			ConsoleWindow( Ped::WindowClosure& closure, ConstStr255Param title );
			
			Io::StringPipe const& Input() const  { return itsInput; }
			Io::StringPipe      & Input()        { return itsInput; }
	};
	
	
	ConsoleWindow::ConsoleWindow( Ped::WindowClosure&  closure,
	                              ConstStr255Param     title   ) : Base( Ped::NewWindowContext( MakeWindowRect(), title ),
	                                                                     closure,
	                                                                     itsInput )
	{
	}
	
	
	const std::string& ConsoleWindowClosure::TTYName() const
	{
		ASSERT( itsTerminal != NULL );
		
		return itsTerminal->TTYName();
	}
	
	static void CloseSalvagedConsole( ConsoleTTYHandle* terminal );
	
	bool ConsoleWindowClosure::RequestWindowClosure( N::WindowRef )
	{
		itHasBeenRequested = true;
		
		if ( !itHasDisassociated )
		{
			SendSignalToProcessGroup( SIGHUP, *itsTerminal->GetProcessGroup().lock().get() );
		}
		else
		{
			CloseSalvagedConsole( itsTerminal );
		}
		
		// Assuming the window does get shut, it hasn't happened yet
		return false;
	}
	
	
	Console::Console( ConsoleTTYHandle* terminal ) : ConsoleWindowClosure  ( terminal                      ),
	                                                 itsLatentTitle        ( DefaultTitle()                ),
	                                                 itsWindowSalvagePolicy( kLampSalvageWindowOnExitNever ),
	                                                 itsLeaderWaitStatus   ( 0                             ),
	                                                 itIsBlocking          ( false                         )
	{
	}
	
	ConsolePane const& Console::Pane() const
	{
		return itsWindow->SubView().ScrolledView();
	}
	
	ConsolePane& Console::Pane()
	{
		return itsWindow->SubView().ScrolledView();
	}
	
	N::Str255 Console::DefaultTitle() const
	{
		return N::Str255( TTYName() );
	}
	
	void Console::SetTitle( ConstStr255Param title )
	{
		itsLatentTitle = title ? title : DefaultTitle();
		
		if ( IsOpen() )
		{
			N::SetWTitle( itsWindow->Get(), itsLatentTitle );
		}
	}
	
	void Console::Open()
	{
		if ( !IsOpen() )
		{
			itsWindow.reset( new ConsoleWindow( *this, itsLatentTitle ) );
		}
	}
	
	static bool ReadyForInputFromWindow( const ConsoleWindow* window )
	{
		return window != NULL  &&  window->Input().Ready();
	}
	
	static std::string ReadInputFromWindow( ConsoleWindow* window )
	{
		ASSERT( window != NULL );
		
		return window->Input().Read();
	}
	
	bool Console::IsReadable() const
	{
		return !itsCurrentInput.empty()  ||  ReadyForInputFromWindow( itsWindow.get() );
	}
	
	int Console::Read( char* data, std::size_t byteCount )
	{
		// Zero byteCount always begets zero result
		if ( byteCount == 0 )
		{
			return 0;
		}
		
		Open();
		
		while ( true )
		{
			if ( itsCurrentInput.empty() )
			{
				if ( ReadyForInputFromWindow( itsWindow.get() ) )
				{
					itsCurrentInput = ReadInputFromWindow( itsWindow.get() );
				}
			}
			
			if ( !itsCurrentInput.empty() )
			{
				// Actual byteCount is lesser of requested size and available size
				std::size_t bytesCopied = std::min( byteCount, itsCurrentInput.size() );
				
				// Copy data from input to buffer
				std::copy( itsCurrentInput.begin(),
				           itsCurrentInput.begin() + bytesCopied,
				           data );
				
				// Slide remaining data to beginning
				std::copy( itsCurrentInput.begin() + bytesCopied,
				           itsCurrentInput.end(),
				           itsCurrentInput.begin() );
				
				// and take up the slack
				itsCurrentInput.resize( itsCurrentInput.size() - bytesCopied );
				
				return bytesCopied;
			}
			
			
			Pane().CheckEOF();
			
			if ( itIsBlocking )
			{
				Yield();
				continue;
			}
			else
			{
				throw io::no_input_pending();
			}
		}
		
		// Not reached?  After a throw?  Who'd have guessed?
		return 0;
	}
	
	int Console::Write( const char* data, std::size_t byteCount )
	{
		Open();
		
		int result = Pane().WriteChars( data, byteCount );
		
		itsWindow->SubView().UpdateScrollbars( N::SetPt( 0, 0 ),
		                                       N::SetPt( 0, 0 ) );
		
		return result;
	}
	
	ConsoleTTYHandle* Console::Salvage()
	{
		SetTitle( N::Str255( "(" + NN::Convert< std::string >( itsLeaderWaitStatus ) + ")" ) );
		
		DisassociateFromTerminal();
		
		return Terminal();
	}
	
	bool Console::ShouldSalvageWindow() const
	{
		return !ClosureHasBeenRequested()  &&  itsLeaderWaitStatus != 0;  // FIXME
	}
	
	boost::shared_ptr< IOHandle > NewConsoleDevice()
	{
		static std::size_t gLastID = 0;
		
		return boost::shared_ptr< IOHandle >( new ConsoleTTYHandle( ++gLastID ) );
	}
	
	void SpawnNewConsole( const FSSpec& program )
	{
		// Create new console/terminal device
		// Spawn new process with file descriptors set
		
		Process& parent = GetProcess( 1 );
		
		Process* external = new Process( parent );
		
		FileDescriptorMap& files = external->FileDescriptors();
		
		// Temporary IORef in nested block goes out of scope prior to Exec().
		// This is necessary because an unforked exec() will lose temporaries.
		{
			boost::shared_ptr< IOHandle > terminal = NewConsoleDevice();
			
			files[ 0 ] = terminal;
			files[ 1 ] = terminal;
			files[ 2 ] = terminal;
		}
		
		std::string programName = NN::Convert< std::string >( program.name );
		
		char const *const argv[] = { programName.c_str(), NULL };
		
		(void) external->Exec( program, argv, NULL );
	}
	
	static FSSpec LoginExecutable()
	{
		return ResolvePathname( "/bin/login", FSTreePtr() )->GetFSSpec();
	}
	
	void SpawnNewConsole()
	{
		return SpawnNewConsole( LoginExecutable() );
	}
	
	
	class ConsolesOwner
	{
		private:
			ConsoleMap map;
		
		public:
			const ConsoleMap& GetMap() const  { return map; }
			
			boost::shared_ptr< Console > NewConsole( ConsoleTTYHandle* terminal );
			
			void CloseConsole( const boost::shared_ptr< Console >& console );
	};
	
	
	boost::shared_ptr< Console > ConsolesOwner::NewConsole( ConsoleTTYHandle* terminal )
	{
		boost::shared_ptr< Console > console( new Console( terminal ) );
		
		map[ console.get() ] = console;
		
		return console;
	}
	
	std::map< ConsoleTTYHandle*, boost::shared_ptr< Console > > gSalvagedConsoles;
	
	void ConsolesOwner::CloseConsole( const boost::shared_ptr< Console >& console )
	{
		if ( console.get() == NULL )
		{
			return;
		}
		
		if ( console->ShouldSalvageWindow() )
		{
			gSalvagedConsoles[ console->Salvage() ] = console;
		}
		
		map.erase( console.get() );
	}
	
	static ConsolesOwner gConsolesOwner;
	
	const ConsoleMap& GetConsoleMap()
	{
		return gConsolesOwner.GetMap();
	}
	
	boost::shared_ptr< Console > NewConsole( ConsoleTTYHandle* terminal )
	{
		return gConsolesOwner.NewConsole( terminal );
	}
	
	void CloseConsole( const boost::shared_ptr< Console >& console )
	{
		gConsolesOwner.CloseConsole( console );
	}
	
	void CloseSalvagedConsole( ConsoleTTYHandle* terminal )
	{
		gSalvagedConsoles.erase( terminal );
	}
	
}

