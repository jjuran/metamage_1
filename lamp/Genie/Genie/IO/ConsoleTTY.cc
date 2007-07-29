/*	=============
 *	ConsoleTTY.cc
 *	=============
 */

#include "Genie/IO/ConsoleTTY.hh"

// Standard C
#include <errno.h>

// Lamp
#include <lamp/winio.h>

// Nucleus
#include "Nucleus/Convert.h"

// POSeve
#include "POSeven/Errno.hh"

// Genie
#include "Genie/Console.hh"
#include "Genie/Yield.hh"


namespace Genie
{
	
	namespace NN = Nucleus;
	namespace P7 = POSeven;
	
	
	static ConsoleMap gConsoleMap;
	
	const ConsoleMap& GetConsoleMap()
	{
		return gConsoleMap;
	}
	
	
	boost::shared_ptr< IOHandle > NewConsoleDevice()
	{
		static ConsoleID gLastID = 0;
		
		boost::shared_ptr< IOHandle > consoleDevice( new ConsoleTTYHandle( ++gLastID ) );
		
		gConsoleMap[ gLastID ] = consoleDevice;
		
		return consoleDevice;
	}
	
	ConsoleTTYHandle& GetConsoleByID( ConsoleID id )
	{
		ConsoleMap::const_iterator it = gConsoleMap.find( id );
		
		if ( it == gConsoleMap.end() )
		{
			P7::ThrowErrno( ENOENT );
		}
		
		ASSERT( !it->second.expired() );
		
		return IOHandle_Cast< ConsoleTTYHandle >( *it->second.lock() );
	}
	
	static std::string MakeConsoleName( ConsoleID id )
	{
		return "/dev/con/" + NN::Convert< std::string >( id );
	}
	
	ConsoleTTYHandle::ConsoleTTYHandle( ConsoleID id ) : TTYHandle( MakeConsoleName( id ) ),
	                                                     id( id ),
	                                                     itsWindowSalvagePolicy( kLampSalvageWindowOnExitNever ),
	                                                     itsLeaderWaitStatus()
	{
	}
	
	static bool ShouldSalvageConsoleWindow( int salvagePolicy, int leaderWaitStatus )
	{
		switch ( salvagePolicy )
		{
			default:
			case kLampSalvageWindowOnExitNever:
				//return false;
			
			case kLampSalvageWindowOnExitForSignal:
				// FIXME
			
			case kLampSalvageWindowOnExitForFailure:
				return leaderWaitStatus != 0;
			
			case kLampSalvageWindowOnExitAlways:
				return true;
		}
	}
	
	ConsoleTTYHandle::~ConsoleTTYHandle()
	{
		if ( console.get()  &&  !IsDisconnected()  &&  ShouldSalvageConsoleWindow( itsWindowSalvagePolicy, itsLeaderWaitStatus ) )
		{
			std::string exCon = "(" + NN::Convert< std::string >( itsLeaderWaitStatus ) + ")";
			
			N::SetWTitle( console->GetWindowRef(), N::Str255( exCon ) );
			
			SalvageConsole( console );
		}
		
		gConsoleMap.erase( id );
	}
	
	unsigned int ConsoleTTYHandle::SysPoll() const
	{
		bool readable = !itsCurrentInput.empty()  ||  console.get() && console->IsReadyForInput();
		
		int readability = readable ? kPollRead : 0;
		
		return readability | kPollWrite;
	}
	
	int ConsoleTTYHandle::SysRead( char* data, std::size_t byteCount )
	{
		Open();
		
		// Zero byteCount always begets zero result
		if ( byteCount == 0 )
		{
			return 0;
		}
		
		while ( true )
		{
			if ( itsCurrentInput.empty() )
			{
				if ( console->IsReadyForInput() )
				{
					itsCurrentInput = console->ReadInput();
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
			
			if ( IsBlocking() )
			{
				Yield();
				continue;
			}
			else
			{
				throw io::no_input_pending();
			}
		}
	}
	
	int ConsoleTTYHandle::SysWrite( const char* data, std::size_t byteCount )
	{
		Open();
		
		return console->Write( data, byteCount );
	}
	
	void ConsoleTTYHandle::IOCtl( unsigned long request, int* argp )
	{
		Open();
		
		switch ( request )
		{
			case WIOCGTITLE:
				if ( argp != NULL )
				{
					N::Str255 title = N::GetWTitle( console->GetWindowRef() );
					
					std::copy( title + 1, title + 1 + title[0], (Byte*) argp );
					
					argp[ title[0] ] = '\0';
				}
				
				break;
			
			case WIOCSTITLE:
				N::SetWTitle( console->GetWindowRef(), argp ? N::Str255( (const char*) argp ) : NULL );
				
				break;
			
			case WIOCGEXIT:
				if ( argp != NULL )
				{
					*argp = itsWindowSalvagePolicy;
				}
				
				break;
			
			case WIOCSEXIT:
				if ( argp == NULL )
				{
					P7::ThrowErrno( EFAULT );
				}
				
				itsWindowSalvagePolicy = *argp;
				//break;
			
			default:
				TTYHandle::IOCtl( request, argp );
				break;
		};
	}
	
	static N::Str255 DefaultConsoleTitle( ConsoleID id )
	{
		return N::Str255( "/dev/con/" + NN::Convert< std::string >( id ) );
	}
	
	static boost::shared_ptr< Console > NewConsole( ConsoleID id, ConstStr255Param title )
	{
		boost::shared_ptr< Console > console( new Console( id, title ) );
		
		return console;
	}
	
	void ConsoleTTYHandle::Open()
	{
		if ( console.get() == NULL )
		{
			console = NewConsole( id, DefaultConsoleTitle( id ) );
		}
	}
	
	void ConsoleTTYHandle::SaveLeaderWaitStatus( int status )
	{
		itsLeaderWaitStatus = status;
	}
	
}

