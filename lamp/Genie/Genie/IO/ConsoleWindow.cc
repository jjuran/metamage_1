/*	=========================
 *	Genie/IO/ConsoleWindow.cc
 *	=========================
 */

#include "Genie/IO/ConsoleWindow.hh"

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


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace Ped = Pedestal;
	
	
	class ConsoleCloseHandler : public Ped::WindowCloseHandler
	{
		private:
			ConsoleID itsConsoleID;
		
		public:
			ConsoleCloseHandler( ConsoleID id ) : itsConsoleID( id )
			{
			}
			
			void operator()( N::WindowRef ) const;
	};
	
	
	bool ConsolePane::CheckEOF()
	{
		if ( itHasReceivedEOF )
		{
			itHasReceivedEOF = false;
			
			return true;
		}
		
		return false;
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
		
		FSTreePtr program = ResolvePathname( "/bin/sh", FSTreePtr() );
		
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
			process->Exit( 127 );
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
				bool deletingLeft = ( c == kBackspaceCharCode ) &&   !textIsSelected;
				bool movingLeft   = ( c == kLeftArrowCharCode ) && ( !textIsSelected || event.modifiers & shiftKey );
				
				bool goingLeft = deletingLeft || movingLeft;
				
				bool ignoring = goingLeft && atStartOfInput;
				
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
	
	
	static boost::shared_ptr< Ped::WindowCloseHandler > NewConsoleCloseHandler( ConsoleID id )
	{
		return boost::shared_ptr< Ped::WindowCloseHandler >( new ConsoleCloseHandler( id ) );
	}
	
	ConsoleWindow::ConsoleWindow( ConsoleID           id,
	                              const std::string&  name ) : Base( Ped::NewWindowContext( MakeWindowRect(),
	                                                                                        N::Str255( name ),
	                                                                                        true ),
	                                                                 NewConsoleCloseHandler( id ),
	                                                                 ConsolePane::Initializer( id, itsInput ) ),
	                                                           WindowHandle( name )
	{
	}
	
	ConsoleWindow::~ConsoleWindow()
	{
	}
	
	bool ConsoleWindow::IsReadyForInput()
	{
		bool ready = itsInput.Ready();
		
		if ( !ready && SubView().ScrolledView().CheckEOF() )
		{
			throw io::end_of_input();
		}
		
		return ready;
	}
	
	int ConsoleWindow::Write( const char* data, std::size_t byteCount )
	{
		int result = SubView().ScrolledView().WriteChars( data, byteCount );
		
		SubView().UpdateScrollbars( N::SetPt( 0, 0 ),
		                            N::SetPt( 0, 0 ) );
		
		return result;
	}
	
	
	void ConsoleCloseHandler::operator()( N::WindowRef ) const
	{
		TerminalHandle& terminal = GetConsoleByID( itsConsoleID );
		
		terminal.Disconnect();
		
		if ( !terminal.GetProcessGroup().expired() )
		{
			SendSignalToProcessGroup( SIGHUP, *terminal.GetProcessGroup().lock() );
		}
	}
	
}

