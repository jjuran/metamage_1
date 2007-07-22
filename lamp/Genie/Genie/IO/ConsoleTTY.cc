/*	=============
 *	ConsoleTTY.cc
 *	=============
 */

#include "Genie/IO/ConsoleTTY.hh"

// Lamp
#include <lamp/winio.h>

// Nucleus
#include "Nucleus/Convert.h"

// Genie
#include "Genie/Console.hh"
#include "Genie/Yield.hh"


namespace Genie
{
	
	namespace NN = Nucleus;
	
	
	static std::string MakeConsoleName( std::size_t id )
	{
		return "/dev/con/" + NN::Convert< std::string >( id );
	}
	
	ConsoleTTYHandle::ConsoleTTYHandle( std::size_t id ) : TTYHandle( MakeConsoleName( id ) ),
	                                                       id( id ),
	                                                       console( NewConsole( this ) )
	{
	}
	
	ConsoleTTYHandle::~ConsoleTTYHandle()
	{
		CloseConsole( console );
	}
	
	unsigned int ConsoleTTYHandle::SysPoll() const
	{
		return (console.get() && console->IsReadable() ? kPollRead
		                                               : 0        ) | kPollWrite;
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
		Process& current = CurrentProcess();
		
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
				*argp = kLampSalvageWindowOnExitNever;
				break;
			
			case WIOCSEXIT:
				
				//break;
			
			default:
				TTYHandle::IOCtl( request, argp );
				break;
		};
	}
	
	void ConsoleTTYHandle::SaveLeaderWaitStatus( int status )
	{
		if ( console.get() != NULL )
		{
			console->SetLeaderWaitStatus( status );
		}
	}
	
}

