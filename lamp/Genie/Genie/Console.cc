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

// Standard C
#include <signal.h>

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
#include "Genie/Devices.hh"
#include "Genie/FileSystem/ResolvePathname.hh"
#include "Genie/IO/ConsoleTTY.hh"
#include "Genie/Process.hh"
#include "Genie/Yield.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace Ped = Pedestal;
	
	
	class ConsoleWindowClosure : public Ped::WindowClosure
	{
		private:
			ConsoleID itsConsoleID;
		
		public:
			ConsoleWindowClosure( ConsoleID id ) : itsConsoleID( id )
			{
			}
			
			ConsoleID ID() const  { return itsConsoleID; }
			
			bool RequestWindowClosure( N::WindowRef );
	};
	
	class SalvagedWindowClosure : public Ped::WindowClosure
	{
		public:
			bool RequestWindowClosure( N::WindowRef window );
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
	
	static void RunShellCommand( const std::string& command )
	{
		const char* argv[] = { "-sh", "-c", "", NULL };
		
		const char* envp[] = { "PATH=/bin:/sbin:/usr/bin:/usr/sbin", NULL };
		
		argv[2] = command.c_str();
		
		FSSpec program = ResolvePathname( "/bin/sh", FSTreePtr() )->GetFSSpec();
		
		Process& parent = GetProcess( 1 );
		
		Process* process = new Process( parent );
		
		FileDescriptorMap& files = process->FileDescriptors();
		
		files[ 0 ] =
		files[ 1 ] =
		files[ 2 ] = GetSimpleDeviceHandle( "console" );
		
		try
		{
			process->Exec( program, argv, envp );
		}
		catch ( ... )
		{
			process->Terminate();
		}
	}
	
	bool ConsolePane::KeyDown( const EventRecord& event )
	{
		char c   =  event.message & charCodeMask;
		char key = (event.message & keyCodeMask) >> 8;
		
		TESelection selection = GetTESelection( Get() );
		
		short& start = selection.start;
		short& end   = selection.end;
		
		if ( c == kEnterCharCode  &&  key >= 0x30 )
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
				
				// This causes the selected text to appear as input for the
				// currently running command, be it shell or something else.
				// It's like typing it in, except that the text doesn't appear.
				// But the shell will prompt afterward and it waits until idle.
				//itsInput.Write( command );
				
				// This runs the command in a new shell immediately.
				RunShellCommand( command );
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
				
				case 'C':
					SendSignalToProcessGroup( SIGINT, *GetConsoleByID( itsConsoleID ).GetProcessGroup().lock() );
					break;
				
				case 'Z':
					SendSignalToProcessGroup( SIGTSTP, *GetConsoleByID( itsConsoleID ).GetProcessGroup().lock() );
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
	
	
	ConsoleWindow::ConsoleWindow( Ped::WindowClosure&  closure,
	                              ConstStr255Param     title,
	                              ConsoleID            id      ) : Base( Ped::NewWindowContext( MakeWindowRect(), title ),
	                                                                     closure,
	                                                                     ConsolePane::Initializer( id, itsInput ) )
	{
	}
	
	static void CloseSalvagedConsole( N::WindowRef window );
	
	bool ConsoleWindowClosure::RequestWindowClosure( N::WindowRef )
	{
		ConsoleTTYHandle& console = GetConsoleByID( itsConsoleID );
		
		console.Disconnect();
		
		SendSignalToProcessGroup( SIGHUP, *console.GetProcessGroup().lock() );
		
		// Assuming the window does get shut, it hasn't happened yet
		return false;
	}
	
	bool SalvagedWindowClosure::RequestWindowClosure( N::WindowRef window )
	{
		CloseSalvagedConsole( window );
		
		return true;
	}
	
	
	static boost::shared_ptr< Ped::WindowClosure > NewConsoleCloseHandler( ConsoleID id )
	{
		return boost::shared_ptr< Ped::WindowClosure >( new ConsoleWindowClosure( id ) );
	}
	
	Console::Console( ConsoleID id, ConstStr255Param title ) : itsConsoleID( id ),
	                                                           itsWindow( *NewConsoleCloseHandler( id ), title, id )
	{
	}
	
	Console::~Console()
	{
	}
	
	static ConsolePane& GetConsolePane( ConsoleWindow& window )
	{
		return window.SubView().ScrolledView();
	}
	
	bool Console::IsReadyForInput()
	{
		bool ready = itsWindow.Input().Ready();
		
		if ( !ready )
		{
			GetConsolePane( itsWindow ).CheckEOF();
		}
		
		return ready;
	}
	
	int Console::Write( const char* data, std::size_t byteCount )
	{
		int result = GetConsolePane( itsWindow ).WriteChars( data, byteCount );
		
		itsWindow.SubView().UpdateScrollbars( N::SetPt( 0, 0 ),
		                                      N::SetPt( 0, 0 ) );
		
		return result;
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
	
	
	std::map< ::WindowRef, boost::shared_ptr< Console > > gSalvagedConsoles;
	
	
	N::WindowRef Console::GetWindowRef() const
	{
		return itsWindow.Get();
	}
	
	void Console::Salvage()
	{
		boost::shared_ptr< Ped::WindowClosure > handler( new SalvagedWindowClosure() );
		
		itsWindow.SetCloseHandler( handler );
	}
	
	void SalvageConsole( const boost::shared_ptr< Console >& console )
	{
		ASSERT( console.get() != NULL );
		
		console->Salvage();
		
		gSalvagedConsoles[ console->GetWindowRef() ] = console;
	}
	
	void CloseSalvagedConsole( N::WindowRef window )
	{
		gSalvagedConsoles.erase( window );
	}
	
}

