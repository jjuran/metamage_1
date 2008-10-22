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

// POSIX
#include <sys/ttycom.h>

// Lamp
#include <lamp/winio.h>

// boost
#include <boost/shared_ptr.hpp>

// Nucleus
#include "Nucleus/NAssert.h"

// Io
#include "io/io.hh"

// Nitrogen
#include "Nitrogen/Processes.h"
#include "Nitrogen/Sound.h"

// POSeven
#include "POSeven/Errno.hh"

// Pedestal
#include "Pedestal/Console.hh"
#include "Pedestal/Scroller.hh"

// Genie
#include "Genie/Devices.hh"
#include "Genie/IO/ConsoleTTY.hh"
#include "Genie/Process.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace p7 = poseven;
	namespace Ped = Pedestal;
	
	
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
		
		Process& parent = GetProcess( 1 );
		
		Process* process = new Process( parent );
		
		FileDescriptorMap& files = process->FileDescriptors();
		
		files[ 0 ] =
		files[ 1 ] =
		files[ 2 ] = GetSimpleDeviceHandle( "console" );
		
		try
		{
			process->Exec( "/bin/sh", argv, envp );
		}
		catch ( ... )
		{
			process->Exit( 127 );
		}
	}
	
	static void SendSignalToProcessGroupForID( int signo, ConsoleID id )
	{
		const boost::shared_ptr< IOHandle >& handle( GetDynamicElementByID< ConsoleTTYHandle >( id ) );
		
		TerminalHandle& terminal( IOHandle_Cast< TerminalHandle >( *handle ) );
		
		SendSignalToProcessGroup( signo, *terminal.GetProcessGroup().lock() );
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
		
		const UInt32 kEitherControlKey = controlKey | rightControlKey;
		
		if ( event.modifiers & kEitherControlKey  &&  c < 0x20 )
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
					SendSignalToProcessGroupForID( SIGINT, itsConsoleID );
					break;
				
				case 'Z':
					SendSignalToProcessGroupForID( SIGTSTP, itsConsoleID );
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
	
	typedef Ped::Scroller< ConsolePane, Ped::kLiveFeedbackVariant > ConsoleView;
	
	inline std::auto_ptr< Ped::View > MakeView( ConsoleID        id,
	                                            Io::StringPipe&  input )
	{
		return std::auto_ptr< Ped::View >( new ConsoleView( MakeWindowRect(),
		                                                    ConsoleView::Initializer( id, input ) ) );
	}
	
	
	ConsoleWindow::ConsoleWindow( ConsoleID           id,
	                              const std::string&  name ) : Base( Ped::NewWindowContext( MakeWindowRect(),
	                                                                                        N::Str255( name ),
	                                                                                        true ),
	                                                                 N::documentProc ),
	                                                           WindowHandle( name )
	{
		SetCloseHandler ( GetDynamicWindowCloseHandler < ConsoleTTYHandle >( id ) );
		SetResizeHandler( GetDynamicWindowResizeHandler< ConsoleTTYHandle >( id ) );
		
		SetView( MakeView( id, itsInput ) );
	}
	
	ConsoleWindow::~ConsoleWindow()
	{
	}
	
	void ConsoleWindow::IOCtl( unsigned long request, int* argp )
	{
		Point* result = (Point*) argp;
		
		switch ( request )
		{
			const Point ptZero = { 0 };
			
			case TIOCGWINSZ:
				if ( result != NULL )
				{
					result[1] = ptZero;
				}
				
				// fall through
			
			case WIOCGDIM:
				if ( result != NULL )
				{
					*result = Ped::ViewableRange( SubView().Get< ConsoleView >().ScrolledView().Get() );
				}
				
				break;
			
			case WIOCSDIM:
				p7::throw_errno( EINVAL );
				break;
			
			default:
				WindowHandle::IOCtl( request, argp );
				break;
		};
	}
	
	bool ConsoleWindow::IsReadyForInput()
	{
		bool ready = itsInput.Ready();
		
		if ( !ready && SubView().Get< ConsoleView >().ScrolledView().CheckEOF() )
		{
			throw io::end_of_input();
		}
		
		return ready;
	}
	
	int ConsoleWindow::Write( const char* data, std::size_t byteCount )
	{
		ConsoleView& view = SubView().Get< ConsoleView >();
		
		int result = view.ScrolledView().WriteChars( data, byteCount );
		
		view.UpdateScrollbars( N::SetPt( 0, 0 ),
		                       N::SetPt( 0, 0 ) );
		
		return result;
	}
	
}

