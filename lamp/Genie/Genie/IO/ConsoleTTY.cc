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
	                                                     console( NewConsole( id ) ),
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
		gConsoleMap.erase( id );
		
		if ( ShouldSalvageConsoleWindow( itsWindowSalvagePolicy, itsLeaderWaitStatus ) )
		{
			std::string exCon = "(" + NN::Convert< std::string >( itsLeaderWaitStatus ) + ")";
			
			console->SetTitle( N::Str255( exCon ) );
			
			SalvageConsole( console );
		}
	}
	
	unsigned int ConsoleTTYHandle::SysPoll() const
	{
		int readability = console->IsReadable() ? kPollRead : 0;
		
		return readability | kPollWrite;
	}
	
	int ConsoleTTYHandle::SysRead( char* data, std::size_t byteCount )
	{
		while ( true )
		{
			try
			{
				return console->Read( data, byteCount );
			}
			catch ( const io::no_input_pending& )
			{
				if ( IsBlocking() )
				{
					Yield();
				}
				else
				{
					throw;
				}
			}
		}
	}
	
	int ConsoleTTYHandle::SysWrite( const char* data, std::size_t byteCount )
	{
		return console->Write( data, byteCount );
	}
	
	void ConsoleTTYHandle::IOCtl( unsigned long request, int* argp )
	{
		switch ( request )
		{
			case WIOCGTITLE:
				if ( argp != NULL )
				{
					N::Str255 title = console->GetTitle();
					
					std::copy( title + 1, title + 1 + title[0], (Byte*) argp );
					
					argp[ title[0] ] = '\0';
				}
				
				break;
			
			case WIOCSTITLE:
				console->SetTitle( argp ? N::Str255( (const char*) argp ) : NULL );
				
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
	
	void ConsoleTTYHandle::SaveLeaderWaitStatus( int status )
	{
		itsLeaderWaitStatus = status;
	}
	
}

